#!/usr/bin/env python3
"""Export a selected AES feature directory into a single consolidated Markdown file.

Proyek ini menggunakan arsitektur C99 AES (Agentic Engineering System).
Feature directories berada di src/<feature>/ dengan konvensi penamaan:
  [layer]_[konsep]_[suffix].[c|h]

Output mencakup:
  - Semua file C/H dalam feature directory yang dipilih
  - File shared yang di-#include dari feature tersebut
  - Header module.<feature>.h jika ada
  - Output lint scan jika lint-arwaky-cli tersedia
"""

import re
import subprocess
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Konstanta & pola regex
# ---------------------------------------------------------------------------

# Layer prefix AES yang valid (dipakai untuk deteksi file AES)
AES_LAYER_PREFIXES = (
    "taxonomy_",
    "contract_",
    "capabilities_",
    "infrastructure_",
    "agent_",
    "surfaces_",
    "root_",
    "module.",
)

# Ekstensi file yang akan dikumpulkan dari feature directory
C_EXTENSIONS = {".c", ".h"}

# Pola #include "shared/..." atau #include "../shared/..."
INCLUDE_SHARED_PATTERN = re.compile(
    r'#\s*include\s+"(?:\.\./)*shared/([^"]+)"'
)

# Sanitasi versi untuk nama file (CWE-22 mitigation)
SAFE_VERSION_CHARS = re.compile(r"[^0-9A-Za-z.\-]")


# ---------------------------------------------------------------------------
# Resolusi workspace
# ---------------------------------------------------------------------------

def resolve_workspace() -> tuple[Path, Path, Path]:
    """Kembalikan (workspace_root, src_dir, docs_finding_dir).

    Keluar dengan error jika direktori src/ tidak ditemukan.
    """
    workspace_root = Path(__file__).resolve().parent.parent
    src_dir = workspace_root / "src"
    docs_finding_dir = workspace_root / ".agents" / "finding"

    if not src_dir.exists():
        print(
            f"Error: direktori 'src' tidak ditemukan di {src_dir}",
            file=sys.stderr,
        )
        sys.exit(1)

    return workspace_root, src_dir, docs_finding_dir


# ---------------------------------------------------------------------------
# Listing feature
# ---------------------------------------------------------------------------

def list_feature_dirs(src_dir: Path) -> list[str]:
    """Daftar subdirektori src/ yang berisi minimal satu file .c atau .h."""
    features: list[str] = []
    for entry in src_dir.iterdir():
        if not entry.is_dir():
            continue
        has_source = any(
            f.suffix in C_EXTENSIONS
            for f in entry.iterdir()
            if f.is_file()
        )
        if has_source:
            features.append(entry.name)
    return sorted(features)


# ---------------------------------------------------------------------------
# Prompt pemilihan feature
# ---------------------------------------------------------------------------

def prompt_feature(features: list[str]) -> str:
    """Tampilkan daftar bernomor, minta pilihan, kembalikan nama feature."""
    print("Feature directory yang tersedia:")
    for i, name in enumerate(features, 1):
        print(f"  {i:2d}) {name}")
    print()

    while True:
        try:
            choice = input(
                f"Pilih feature (1-{len(features)}) atau 'q' untuk keluar: "
            ).strip()
        except (EOFError, KeyboardInterrupt):
            print("\nKeluar.")
            sys.exit(0)

        if choice.lower() == "q":
            print("Keluar.")
            sys.exit(0)

        try:
            idx = int(choice) - 1
        except ValueError:
            print("Error: Input tidak valid. Masukkan angka atau 'q'.")
            continue

        if 0 <= idx < len(features):
            return features[idx]

        print(f"Error: Pilih angka antara 1 dan {len(features)}.")


# ---------------------------------------------------------------------------
# Deteksi versi
# ---------------------------------------------------------------------------

def read_project_version(workspace_root: Path, fallback: str = "0.1.0") -> str:
    """Baca versi proyek dari Makefile atau file VERSION.

    Urutan pencarian:
      1. Makefile — baris ``VERSION ?= x.y.z`` atau ``VERSION = x.y.z``
      2. File VERSION di workspace root
      3. fallback
    """
    makefile = workspace_root / "Makefile"
    if makefile.exists():
        try:
            content = makefile.read_text(encoding="utf-8", errors="replace")
            m = re.search(r"^VERSION\s*[\?:]?=\s*([^\s#]+)", content, re.MULTILINE)
            if m:
                return m.group(1).strip()
        except OSError:
            pass

    version_file = workspace_root / "VERSION"
    if version_file.exists():
        try:
            ver = version_file.read_text(encoding="utf-8").strip()
            if ver:
                return ver
        except OSError:
            pass

    return fallback


def sanitize_version(version: str) -> str:
    """Hapus karakter yang bisa melarikan diri dari direktori output (CWE-22)."""
    safe = SAFE_VERSION_CHARS.sub("_", version)
    return safe or "0.0.0"


# ---------------------------------------------------------------------------
# Indexing shared
# ---------------------------------------------------------------------------

def index_shared_dir(shared_dir: Path) -> dict[str, Path]:
    """Buat peta {basename → Path} untuk semua file C/H di src/shared/.

    Digunakan untuk meresolusi path nyata dari baris #include "shared/...".
    """
    index: dict[str, Path] = {}
    if not shared_dir.exists():
        print(
            "Warning: direktori 'src/shared' tidak ditemukan. "
            "Dependensi shared tidak dapat diresolusi."
        )
        return index

    for f in shared_dir.rglob("*"):
        if f.is_file() and f.suffix in C_EXTENSIONS:
            index[f.name] = f
    return index


# ---------------------------------------------------------------------------
# Pengumpulan file feature
# ---------------------------------------------------------------------------

def collect_feature_files(feature_dir: Path) -> set[Path]:
    """Kumpulkan semua file .c dan .h (plus README/FRD) dari feature directory."""
    files: set[Path] = set()
    important_extras = {"README.md", "FRD.md"}

    for f in feature_dir.iterdir():
        if not f.is_file():
            continue
        if f.suffix in C_EXTENSIONS:
            files.add(f)
        elif f.name in important_extras:
            files.add(f)

    return files


# ---------------------------------------------------------------------------
# Resolusi #include shared
# ---------------------------------------------------------------------------

def resolve_shared_includes(
    files: set[Path],
    shared_index: dict[str, Path],
) -> set[Path]:
    """Scan file-file feature, temukan #include "shared/..." dan tambahkan ke set.

    Transitive: jika file shared juga meng-include shared lainnya,
    file tersebut turut ditambahkan.
    """
    print("Memindai #include shared dari file feature...")
    resolved: set[Path] = set()
    queue: list[Path] = list(files)
    visited: set[Path] = set(files)

    while queue:
        current = queue.pop()
        if current.suffix not in C_EXTENSIONS:
            continue
        try:
            content = current.read_text(encoding="utf-8", errors="replace")
        except OSError as e:
            print(f"  Warning: Gagal membaca {current} ({e})")
            continue

        for match in INCLUDE_SHARED_PATTERN.finditer(content):
            included_name = Path(match.group(1)).name  # basename saja
            target = shared_index.get(included_name)
            if target and target not in visited:
                visited.add(target)
                resolved.add(target)
                queue.append(target)  # transitive scan

    if resolved:
        print(f"  -> {len(resolved)} file shared ditemukan.")
    else:
        print("  -> Tidak ada dependensi shared ditemukan.")

    return resolved


# ---------------------------------------------------------------------------
# Lint scan
# ---------------------------------------------------------------------------

def run_lint_scan(workspace_root: Path, feature_dir: Path) -> str:
    """Jalankan lint-arwaky-cli scan pada feature directory jika tersedia."""
    import shutil

    cli_bin: str | None = shutil.which("lint-arwaky-cli")
    if not cli_bin:
        for build_type in ("release", "debug"):
            candidate = workspace_root / "target" / build_type / "lint-arwaky-cli"
            if candidate.exists():
                cli_bin = str(candidate)
                break

    if not cli_bin:
        print(
            "Warning: lint-arwaky-cli tidak ditemukan. "
            "Jalankan install.local.sh terlebih dahulu."
        )
        return ""

    try:
        result = subprocess.run(
            [cli_bin, "scan", str(feature_dir)],
            capture_output=True,
            text=True,
            timeout=120,
            cwd=str(workspace_root),
        )
        output = result.stdout + result.stderr
        lines = [line.rstrip() for line in output.splitlines() if line.strip()]
        return "\n".join(lines) if lines else ""
    except subprocess.TimeoutExpired:
        print("Warning: Lint scan timeout setelah 120s.")
        return ""
    except OSError as e:
        print(f"Warning: Gagal menjalankan lint scan ({e}).")
        return ""


# ---------------------------------------------------------------------------
# Penulisan Markdown
# ---------------------------------------------------------------------------

def _language_for(path: Path) -> str:
    """Kembalikan identifier bahasa untuk fenced code block berdasarkan ekstensi."""
    if path.suffix in (".h", ".c"):
        return "c"
    if path.suffix == ".md":
        return "markdown"
    if path.name == "Makefile":
        return "makefile"
    return "text"


def _aes_layer_of(path: Path) -> str:
    """Deteksi layer AES dari nama file untuk dipakai sebagai label."""
    name = path.name
    for prefix in AES_LAYER_PREFIXES:
        if name.startswith(prefix):
            return prefix.rstrip("_.").rstrip("_")
    return "unknown"


def write_markdown(
    output_path: Path,
    feature_name: str,
    all_files: list[Path],
    sorted_feature: list[Path],
    sorted_shared: list[Path],
    workspace_root: Path,
    safe_version: str,
    lint_output: str,
) -> None:
    """Tulis consolidated Markdown ke output_path."""
    with open(output_path, "w", encoding="utf-8") as out:
        # --- Header ---
        out.write(f"# Feature: `{feature_name}` (v{safe_version})\n\n")
        out.write(
            f"Dokumen ini berisi source code fitur `{feature_name}` beserta "
            "definisi shared yang diimport oleh feature tersebut.\n\n"
        )
        out.write(
            "Arsitektur: **AES C99** — layer prefix: "
            "`taxonomy_` -> `contract_` -> `capabilities_` / `infrastructure_` "
            "-> `agent_` -> `surfaces_` -> `root_`\n\n"
        )

        # --- Lint ---
        if lint_output:
            out.write("## Problem Statement\n\n")
            out.write(
                "Isu berikut terdeteksi oleh `lint-arwaky-cli scan`:\n\n"
            )
            out.write("```\n")
            out.write(lint_output)
            if not lint_output.endswith("\n"):
                out.write("\n")
            out.write("```\n\n---\n\n")

        # --- Daftar file ---
        out.write("## Daftar File\n\n")

        if sorted_feature:
            out.write(f"### Feature: `src/{feature_name}/`\n\n")
            for f in sorted_feature:
                rel = f.relative_to(workspace_root)
                layer = _aes_layer_of(f)
                out.write(f"- [{rel}]({f.as_uri()}) — layer: `{layer}`\n")
            out.write("\n")

        if sorted_shared:
            out.write("### Shared (`src/shared/`)\n\n")
            for f in sorted_shared:
                rel = f.relative_to(workspace_root)
                layer = _aes_layer_of(f)
                out.write(f"- [{rel}]({f.as_uri()}) — layer: `{layer}`\n")
            out.write("\n")

        out.write("---\n\n")

        # --- Isi file ---
        for f in all_files:
            rel = f.relative_to(workspace_root)
            layer = _aes_layer_of(f)
            out.write(f"## `{rel}`\n\n")
            out.write(f"> **Layer AES:** `{layer}`\n\n")
            lang = _language_for(f)
            out.write(f"```{lang}\n")
            try:
                content = f.read_text(encoding="utf-8", errors="replace")
                escaped = content.replace("```", "``` `")
                out.write(escaped)
                if not content.endswith("\n"):
                    out.write("\n")
            except OSError as e:
                out.write(f"/* Error membaca file: {e} */\n")
            out.write("```\n\n---\n\n")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    while True:
        print("\n=== AES Feature Exporter (C99) ===")

        workspace_root, src_dir, docs_finding_dir = resolve_workspace()

        features = list_feature_dirs(src_dir)
        if not features:
            print(
                "Error: Tidak ada feature directory ditemukan di 'src/'.",
                file=sys.stderr,
            )
            sys.exit(1)

        selected = prompt_feature(features)
        print(f"\nMemproses feature: {selected}...")

        # Versi
        version = read_project_version(workspace_root)
        safe_version = sanitize_version(version)
        print(f"Versi proyek: {version} (nama file: {safe_version})")

        # Kumpulkan file feature
        feature_dir = src_dir / selected
        feature_files = collect_feature_files(feature_dir)
        print(f"File feature ditemukan: {len(feature_files)}")

        # Index shared & resolve include
        shared_dir = src_dir / "shared"
        shared_index = index_shared_dir(shared_dir)
        shared_files = resolve_shared_includes(feature_files, shared_index)

        # Susun semua file: feature dulu (sorted), lalu shared (sorted)
        sorted_feature = sorted(feature_files)
        sorted_shared = sorted(shared_files)
        all_files = sorted_feature + sorted_shared

        # Lint scan
        print("Menjalankan lint-arwaky-cli scan...")
        lint_output = run_lint_scan(workspace_root, feature_dir)
        if lint_output:
            print("Lint scan selesai.")
        else:
            print("Tidak ada output lint (bersih atau scan tidak tersedia).")

        # Tulis output
        docs_finding_dir.mkdir(parents=True, exist_ok=True)
        output_filename = f"{selected}_v{safe_version}.md"
        output_path = docs_finding_dir / output_filename

        print(f"Menulis export ke {output_path}...")
        write_markdown(
            output_path,
            selected,
            all_files,
            sorted_feature,
            sorted_shared,
            workspace_root,
            safe_version,
            lint_output,
        )

        print(f"\nBerhasil! File Markdown konsolidasi dibuat: {output_path}")

        try:
            again = input("\nExport feature lain? (y/n): ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            print("\nKeluar.")
            break

        if again != "y":
            break

    print("Selesai.")


if __name__ == "__main__":
    main()
