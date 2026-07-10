#!/usr/bin/env python3
"""
ci_local.py — Local CI/CD gate satu perintah untuk aplikasi-penalti.

Menjalankan berurutan:
  1. aes_lint.py   (enforce RULES_AES.md)
  2. make test     (unit test + invarian + mutation-gate-ready)
  3. make lint     (cppcheck)
  4. make coverage (opsional, butuh --coverage)
  5. mutation_test.py (opsional, butuh --mutation)

Berhenti di step pertama yang gagal (fail-fast). Exit code 0 = hijau.
Cara pakai:
  python3 scripts/ci_local.py
  python3 scripts/ci_local.py --coverage --mutation
  python3 scripts/ci_local.py --fail-on CRITICAL
"""
import argparse, os, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def step(name, cmd, cwd=ROOT):
    print(f"\n{'='*60}\n>>> {name}\n    $ {cmd}\n{'='*60}")
    r = subprocess.run(cmd, shell=True, cwd=cwd)
    if r.returncode != 0:
        print(f"\n[FAIL] {name} exit={r.returncode}")
        sys.exit(r.returncode)
    print(f"[OK] {name}")
    return r.returncode

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--coverage", action="store_true", help="jalankan make coverage")
    ap.add_argument("--mutation", action="store_true", help="jalankan mutation_test.py")
    ap.add_argument("--fail-on", default="HIGH", help="threshold aes_lint")
    args = ap.parse_args()

    # 1. AES lint
    step("AES Lint (RULES_AES.md)", f"python3 scripts/aes_lint.py src/ --fail-on {args.fail_on}")
    # 2. Unit test
    step("Unit Test", "make clean >/dev/null 2>&1 && make test")
    # 3. Cppcheck
    step("Static Analysis (cppcheck)", "make lint")
    # 4. Coverage (optional)
    if args.coverage:
        step("Coverage", "make coverage")
    # 5. Mutation (optional)
    if args.mutation:
        step("Mutation Gate", "python3 mutation_test.py")

    print("\n" + "="*60)
    print("  LOCAL CI: SEMUA HIJAU ✅")
    print("="*60)
    sys.exit(0)

if __name__ == "__main__":
    main()
