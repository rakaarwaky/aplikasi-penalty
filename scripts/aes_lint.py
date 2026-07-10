#!/usr/bin/env python3
"""
aes_lint.py — Custom linter penegak aturan AES (RULES_AES.md) untuk repo C.

Repo ini C murni, jadi rule diadaptasi:
  - Layer diidentifikasi dari PREFIX filename (taxonomy_, contract_, capabilities_,
    infrastructure_, agent_, surfaces_, root_) + fallback direktori.
  - Include di-map ke layer dari basename path include.

Rule yang DITERAPKAN (terukur di C):
  AES101 naming, AES102 suffix, AES201 forbidden import (CRITICAL),
  AES202 mandatory import, AES203 unused import, AES301/302 file size,
  AES303 mandatory definition, AES304 bypass comment (CRITICAL), AES205 circular import.

Exit code: 0 = bersih (atau hanya violation di bawah --fail-on),
           1 = ada violation >= --fail-on (CI gate gagal).

Cara pakai:
  python3 scripts/aes_lint.py src/
  python3 scripts/aes_lint.py src/ --fail-on HIGH
  python3 scripts/aes_lint.py src/ --report markdown
"""
import os, re, sys, argparse

# ---------- Layer detection ----------
STRICT_SUFFIX = {
    "root":          (["_entry", "_container"], []),
    "taxonomy":      (["_vo","_entity","_error","_event","_constant","_utility","_helper","_logger","_log_level"], []),
    "contract":      (["_port","_protocol","_aggregate"], []),
    "surfaces":      (["_command","_controller","_page","_view","_component","_router",
                       "_layout","_hook","_store","_action","_screen"], []),
    "agent":         (["_orchestrator"], []),
}
FLEXIBLE_SUFFIX = {
"capabilities":   (["_checker","_analyzer","_processor","_evaluator","_resolver","_validator",
                    "_formatter","_executor","_transformer","_calculator","_builder","_compiler",
                    "_aggregator","_classifier","_extractor","_reporter","_mapper","_filter",
                    "_collector","_comparator","_scorer","_inspector","_reviewer","_assessor","_auditor",
                    "_appender","_resolver"],
                   ["_vo","_entity","_error","_event","_constant","_constants","_utility","_helper",
                    "_port","_protocol","_aggregate"]),
    "infrastructure": (["_adapter","_provider","_scanner","_client","_schemas","_lifespan","_wrapper",
                        "_tracer","_tracker","_variants","_detector","_patterns","_system","_repository",
                        "_cache","_loader","_writer","_reader","_driver","_connector","_gateway","_serializer",
                        "_encoder","_decoder","_fetcher","_watcher","_indexer","_dispatcher","_recorder",
                        "_proxy","_publisher","_subscriber","_listener","_poller","_streamer"],
                       ["_vo","_entity","_error","_event","_constant","_constants","_utility","_helper",
                        "_port","_protocol","_aggregate"]),
}
EXEMPT_NAMES = {
    "root_cli_main_entry.c", "root_mcp_main_entry.c", "root_tui_main_entry.c",
    "root_composition_container.c", "module.cli.h", "module.registration.h",
    "module.scoring.h", "module.ranking.h", "module.search.h", "module.recap.h",
    "module.storage.h", "module.export.h", "module.sanitizer.h",
}

def layer_of(filename, dirpath):
    base = os.path.basename(filename)
    name = base[:-2] if base.endswith((".c",".h")) else base
    low = name.lower()
    for pfx in ("taxonomy_","contract_","capabilities_","infrastructure_","agent_","surfaces_","root_"):
        if low.startswith(pfx):
            return pfx[:-1]  # taxonomy, contract, capabilities, infrastructure, agent, surfaces, root
    d = os.path.basename(os.path.normpath(dirpath))
    if d == "shared": return "taxonomy"   # default; diperbaiki oleh prefix di include
    if d == "cli": return "surfaces"
    if d == "tui": return "infrastructure"
    return "capabilities"  # fallback netral

def layer_of_include(inc):
    base = os.path.basename(inc)
    name = base[:-2] if base.endswith((".c",".h")) else base
    low = name.lower()
    # barrel/aggregate module.*.h bersifat netral (sudah mewakili taxonomy+contract)
    if low.startswith("module."):
        return None
    for pfx in ("taxonomy_","contract_","capabilities_","infrastructure_","agent_","surfaces_","root_"):
        if low.startswith(pfx):
            return pfx[:-1]
    if "shared/" in inc or inc.startswith("shared/"):
        if "contract_" in low: return "contract"
        if "taxonomy_" in low: return "taxonomy"
        return "taxonomy"
    return None

# ---------- AES201 forbidden import matrix ----------
# key = layer file saat ini; value = set layer yang DILARANG di-include
FORBIDDEN = {
    "taxonomy":       {"agent","infrastructure","surfaces","contract","capabilities","root"},
    "contract":       {"agent","infrastructure","surfaces","capabilities","root"},
    "capabilities":   {"infrastructure","surfaces","agent","capabilities","root"},
    "infrastructure": {"surfaces","capabilities","agent","infrastructure","root"},
    "agent":          {"surfaces","infrastructure","capabilities","root"},
    "surfaces":       {"agent","infrastructure","capabilities","root"},
    "root":           set(),
}
# AES202 mandatory include
MANDATORY = {
    "capabilities":   ["taxonomy","contract"],   # butuh protocol
    "infrastructure": ["taxonomy","contract"],   # butuh port
    "agent":          ["taxonomy","contract"],
    "surfaces":       ["taxonomy","contract"],
    "contract":       ["taxonomy"],
    "taxonomy":       [],
    "root":           [],
}

# ---------- helpers ----------
def read_lines(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        return f.read().splitlines()

def find_includes(lines):
    out = []
    for i, ln in enumerate(lines, 1):
        m = re.search(r'#\s*include\s*[<"]([^>"]+)[>"]', ln)
        if m:
            out.append((i, m.group(1)))
    return out

def get_definitions(lines):
    pat = re.compile(r'\b(struct|enum|union|typedef)\b')
    return sum(1 for ln in lines if pat.search(ln))

# ---------- severity ordering ----------
SEV_ORDER = {"LOW":0,"MEDIUM":1,"HIGH":2,"CRITICAL":3}

class Violation:
    def __init__(self, code, sev, path, line, msg):
        self.code, self.sev, self.path, self.line, self.msg = code, sev, path, line, msg
    def __str__(self):
        loc = f"{self.path}:{self.line}" if self.line else self.path
        return f"[{self.sev:8}] {self.code:6} {loc:45} {self.msg}"

def check_file(path, dirpath, violations):
    base = os.path.basename(path)
    if base in EXEMPT_NAMES or base.startswith("__init__"):
        return
    lines = read_lines(path)
    layer = layer_of(base, dirpath)
    name = base[:-2] if base.endswith((".c",".h")) else base

    # --- AES101 naming ---
    if layer != "taxonomy":  # taxonomy di shared/ dikecualikan pola prefix
        if not re.fullmatch(r"[a-z][a-z0-9]*(_[a-z0-9]+)+", name):
            violations.append(Violation("AES101","HIGH",path,0,
                f"nama '{name}' tidak mengikuti prefix_concept_suffix (lowercase, underscore, >=2 kata)"))

    # --- AES102 suffix ---
    if layer in STRICT_SUFFIX:
        allowed, forbidden = STRICT_SUFFIX[layer]
        ok = any(name.endswith(s) for s in allowed)
        bad = any(name.endswith(s) for s in forbidden)
        if bad:
            violations.append(Violation("AES102","HIGH",path,0,f"suffix terlarang untuk layer {layer}"))
        elif not ok:
            violations.append(Violation("AES102","HIGH",path,0,
                f"suffix '{name}' tidak diizinkan layer {layer}; harus salah satu: {allowed}"))
    elif layer in FLEXIBLE_SUFFIX:
        allowed, forbidden = FLEXIBLE_SUFFIX[layer]
        if any(name.endswith(s) for s in forbidden):
            violations.append(Violation("AES102","HIGH",path,0,f"suffix terlarang untuk layer {layer}"))
        elif not any(name.endswith(s) for s in allowed):
            violations.append(Violation("AES102","MEDIUM",path,0,
                f"suffix '{name}' tidak dikenali layer {layer}; disarankan: {allowed[:5]}..."))

    # --- AES301/302 size ---
    n = len(lines)
    if n > 1000:
        violations.append(Violation("AES301","LOW",path,0,f"file {n} baris (>1000)"))
    if n < 5 and not base.endswith(".h"):
        violations.append(Violation("AES302","LOW",path,0,f"file hanya {n} baris (<5)"))

    # --- AES303 definition ---
    if (get_definitions(lines) == 0 and not base.endswith(".h")
            and layer not in ("surfaces","agent","capabilities","infrastructure")
            and name not in ("taxonomy_logger",) and not name.startswith("root_")):
        violations.append(Violation("AES303","HIGH",path,0,"tidak ada struct/enum/union/typedef"))

    # --- AES304 bypass comment ---
    for i, ln in enumerate(lines, 1):
        if re.search(r'noqa|NOLINT|lint-disable|type:\s*ignore|eslint-disable', ln, re.I):
            violations.append(Violation("AES304","CRITICAL",path,i,
                "bypass comment terdeteksi (noqa/NOLINT/lint-disable)"))

    # --- includes ---
    incs = find_includes(lines)
    seen_layers = set()
    self_base = name  # untuk deteksi self-header
    for ln, inc in incs:
        incl_layer = layer_of_include(inc)
        inc_base = os.path.basename(inc)[:-2] if os.path.basename(inc).endswith((".c",".h")) else os.path.basename(inc)
        if incl_layer:
            seen_layers.add(incl_layer)
        # AES201: skip self header (.c include .h pasangannya)
        if inc_base == self_base:
            continue
        # AES201: module.*.h netral, tak masuk forbidden
        if layer in FORBIDDEN and incl_layer in FORBIDDEN[layer]:
            violations.append(Violation("AES201","CRITICAL",path,ln,
                f"{layer} meng-include layer terlarang '{incl_layer}' ({inc})"))
        # AES203 unused (heuristic): skip barrel module.*.h (penggunaan simbol tak cocok nama file)
        if inc_base.lower().startswith("module."):
            continue
        hbase = os.path.basename(inc)
        sym = hbase[:-2] if hbase.endswith(".h") else hbase
        sym_clean = re.sub(r'[^a-zA-Z0-9_]', '_', sym)
        if sym_clean and sym_clean not in " ".join(lines):
            violations.append(Violation("AES203","MEDIUM",path,ln,
                f"include '{inc}' sepertinya tidak dipakai (simbol '{sym_clean}' tak ditemukan)"))

    # --- AES202 mandatory ---
    # module.*.h bersifat netral -> dianggap sudah memenuhi taxonomy+contract
    has_module_barrel = any(os.path.basename(inc).lower().startswith("module.") for _, inc in incs)
    # .c yang include header pasangannya sudah membawa dependency via header -> exempt
    includes_self_header = any(os.path.basename(inc)[:-2] == self_base for _, inc in incs) if not base.endswith(".h") else False
    for need in MANDATORY.get(layer, []):
        if need in seen_layers:
            continue
        if has_module_barrel or includes_self_header:
            continue  # dependency dibawa barrel/header pasangan
        violations.append(Violation("AES202","HIGH",path,0,f"layer {layer} wajib #include {need}"))

    return incs

def build_graph(root, violations):
    """Return (graph, file_layer). Deteksi AES205 circular."""
    graph = {}
    file_layer = {}
    files = []
    for dp, _, fs in os.walk(root):
        for f in fs:
            if f.endswith((".c",".h")):
                files.append((os.path.join(dp, f), dp))
    for path, dp in files:
        base = os.path.basename(path)
        if base in EXEMPT_NAMES:
            continue
        lines = read_lines(path)
        layer = layer_of(base, dp)
        file_layer[path] = layer
        incs = [inc for _, inc in find_includes(lines)]
        deps = []
        for inc in incs:
            # resolve to a real file if possible
            cand = os.path.join(dp, os.path.basename(inc))
            if not os.path.exists(cand):
                cand = os.path.join(root, inc)
            if os.path.exists(cand):
                deps.append(cand)
        graph[path] = deps
    # cycle detect (DFS)
    WHITE, GRAY, BLACK = 0,1,2
    color = {n:WHITE for n in graph}
    def dfs(u, stack):
        color[u] = GRAY
        for v in graph.get(u, []):
            if v not in color: continue
            if color[v] == GRAY:
                cyc = stack + [v]
                violations.append(Violation("AES205","HIGH",v,0,
                    "circular import: " + " -> ".join(os.path.basename(x) for x in cyc)))
                return
            if color[v] == WHITE:
                dfs(v, stack+[u])
        color[u] = BLACK
    for n in graph:
        if color[n] == WHITE:
            dfs(n, [])
    return graph, file_layer

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("path", nargs="?", default="src")
    ap.add_argument("--fail-on", default="HIGH", choices=list(SEV_ORDER))
    ap.add_argument("--report", default="text", choices=["text","markdown"])
    args = ap.parse_args()

    violations = []
    for dp, _, fs in os.walk(args.path):
        for f in sorted(fs):
            if f.endswith((".c",".h")):
                check_file(os.path.join(dp, f), dp, violations)
    build_graph(args.path, violations)

    # sort by severity then code
    violations.sort(key=lambda v: (-SEV_ORDER[v.sev], v.code, v.path))

    if args.report == "markdown":
        print(f"# AES Lint Report — `{args.path}`\n")
        print(f"**Total violations:** {len(violations)}\n")
        cur = None
        for v in violations:
            if v.sev != cur:
                cur = v.sev
                print(f"\n## {cur}\n")
            loc = f"{v.path}:{v.line}" if v.line else v.path
            print(f"- `{v.code}` {loc} — {v.msg}")
    else:
        print(f"=== AES Lint: {args.path} ===")
        for v in violations:
            print(v)
        print(f"\nTotal violations: {len(violations)}")

    # threshold
    gate = sum(1 for v in violations if SEV_ORDER[v.sev] >= SEV_ORDER[args.fail_on])
    print(f"Violations >= {args.fail_on}: {gate}")
    sys.exit(1 if gate else 0)

if __name__ == "__main__":
    main()
