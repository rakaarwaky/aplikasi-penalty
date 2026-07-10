#!/usr/bin/env python3
"""
mutation_test.py — Mutation testing gate untuk aplikasi-penalty.

Cara kerja:
  Untuk tiap mutan: backup file -> patch (1 token) -> rebuild test binary
  secara incremental -> jalankan ./run_tests. Jika test GAGAL, mutan
  "killed" (gate menangkap bug). Jika test hijau, "survived". Lalu restore.

Metrik:  mutation score = killed / total  (semakin tinggi, gate makin kuat)
"""
import subprocess, os, sys, shutil

ROOT = "/home/raka/Study/PengantarCoding/aplikasi-penalty"
os.chdir(ROOT)
SRC = "src"
TEST_BIN = "run_tests"

MUTANTS = [
    # --- boundary / relational flip ---
    ("scoring/capabilities_scoring_zone_validator.c",
     "zone.value < MIN_ZONE", "zone.value <= MIN_ZONE",
     "zona: terima batas bawah (0 jadi invalid)"),
    ("scoring/capabilities_scoring_zone_validator.c",
     "zone.value > MAX_ZONE", "zone.value >= MAX_ZONE",
     "zona: tolak batas atas (5 jadi invalid)"),
    ("registration/capabilities_registration_validator.c",
     "len > (size_t)MAX_NAME_LENGTH", "len >= (size_t)MAX_NAME_LENGTH",
     "nama: batas panjang longgar 1 karakter"),
    ("registration/capabilities_registration_appender.c",
     "state->participant_count >= MAX_PARTICIPANTS",
     "state->participant_count > MAX_PARTICIPANTS",
     "kuota: izinkan 1 peserta lewat batas"),
    ("sanitizer/capabilities_sanitizer_validator.c",
     "val < min_val", "val <= min_val",
     "sanitizer int: terima nilai di bawah min"),
    ("scoring/capabilities_scoring_score_calculator.c",
     "p->kick_count.value >= TOTAL_KICKS",
     "p->kick_count.value > TOTAL_KICKS",
     "scoring: izinkan 1 tendangan lewat batas"),
    ("registration/capabilities_registration_validator.c",
     "else if (c != ' ') return REG_NAME_INVALID_CHAR;",
     "else if (c == ' ') return REG_NAME_INVALID_CHAR;",
     "nama: karakter spasi dianggap invalid (logika terbalik)"),
    ("sanitizer/capabilities_sanitizer_validator.c",
     "if (*endptr != '\\0' || endptr == input) {",
     "if (*endptr == '\\0' || endptr == input) {",
     "sanitizer: empty/non-numeric tidak lagi ditolak"),
    ("storage/infrastructure_storage_adapter.c",
     "if (file == NULL) {",
     "if (file != NULL) {",
     "storage save: file gagal dibuka malah dianggap OK"),
    ("export/infrastructure_export_adapter.c",
     "if (file == NULL) {",
     "if (file != NULL) {",
     "export: file gagal dibuka malah dianggap OK"),
    ("recap/capabilities_recap_formatter.c",
     "if (state == NULL || details == NULL) return RC_NOT_READY;",
     "if (state == NULL || details != NULL) return RC_NOT_READY;",
     "recap: details NULL tidak lagi ditolak"),
    # --- return-code swap ---
    ("ranking/capabilities_ranking_calculator.c",
     "if (x->total_score != y->total_score) return y->total_score - x->total_score;",
     "if (x->total_score != y->total_score) return x->total_score - y->total_score;",
     "ranking: urutan sortir terbalik (ascending)"),
    ("scoring/capabilities_scoring_zone_validator.c",
     "if (zone.value < MIN_ZONE || zone.value > MAX_ZONE) return SC_INVALID_ZONE;",
     "if (zone.value < MIN_ZONE || zone.value > MAX_ZONE) return SC_OK;",
     "zona: nilai invalid dikembalikan OK"),
    ("ranking/capabilities_ranking_calculator.c",
     "return RK_OK;", "return RK_NOT_READY;",
     "ranking: selalu return NOT_READY"),
    ("sanitizer/capabilities_sanitizer_validator.c",
     "return SANITIZE_OK;", "return SANITIZE_ERROR_TOO_LONG;",
     "sanitizer: input valid dikembalikan error"),
    ("storage/infrastructure_storage_adapter.c",
     "if (written != 1 || fclose(file) != 0) {",
     "if (written != 1 || fclose(file) == 0) {",
     "storage: fclose error diabaikan (false-success)"),
]

def run(cmd):
    return subprocess.run(cmd, shell=True, capture_output=True, text=True)

def build_only():
    r = run(f"make {TEST_BIN} >/dev/null 2>&1")
    return r.returncode

def run_only():
    r = run(f"./{TEST_BIN} >/dev/null 2>&1")
    return r.returncode

def main():
    print("=== MUTATION TESTING GATE ===")
    print("[baseline] build + test normal...", end=" ")
    if build_only() != 0 or run_only() != 0:
        print("GAGAL — perbaiki dulu sebelum mutation test")
        sys.exit(1)
    print("hijau.\n")

    killed, survived, buildfail, skipped = [], [], [], []
    for i, (rel, old, new, desc) in enumerate(MUTANTS, 1):
        path = os.path.join(SRC, rel)
        bak = path + ".mutbak"
        restored = False
        if not os.path.exists(path):
            print(f"[{i}/{len(MUTANTS)}] SKIP (file tak ada): {desc}")
            skipped.append(desc)
            continue
        shutil.copy(path, bak)
        try:
            s = open(path).read()
            if old not in s:
                print(f"[{i}/{len(MUTANTS)}] SKIP (pola tak ditemukan): {desc}")
                skipped.append(desc)
                os.remove(bak)
                continue
            open(path, "w").write(s.replace(old, new, 1))
            os.utime(path, None)  # paksa make rebuild
            br = build_only()
            if br != 0:
                status = "BUILD_FAIL"
            else:
                status = "PASS" if run_only() == 0 else "FAIL"
        finally:
            if os.path.exists(bak):
                shutil.move(bak, path)
                os.utime(path, None)
                build_only()  # kembalikan binary normal
                restored = True

        tag = {"PASS": "SURVIVED", "FAIL": "KILLED",
               "BUILD_FAIL": "BUILD_FAIL"}[status]
        print(f"[{i}/{len(MUTANTS)}] {tag:10} | {desc}")
        if status == "FAIL": killed.append(desc)
        elif status == "PASS": survived.append(desc)
        else: buildfail.append(desc)

    total = len(MUTANTS) - len(skipped)
    score = len(killed) / total * 100 if total else 0
    print("\n=== HASIL ===")
    print(f"Total mutan    : {total}  (skip {len(skipped)})")
    print(f"Killed (gate nangkep) : {len(killed)}")
    print(f"Survived (lolos)       : {len(survived)}")
    print(f"Build fail             : {len(buildfail)}")
    print(f"MUTATION SCORE : {score:.1f}%")
    if survived:
        print("\nMutan yang SELAMAT (butuh test tambahan):")
        for d in survived: print(f"  - {d}")
    if buildfail:
        print("\nMutan BUILD FAIL (perlu cek):")
        for d in buildfail: print(f"  - {d}")
    # pastikan repo kembali hijau
    build_only(); run_only()
    return 0 if score >= 80 else 1

if __name__ == "__main__":
    sys.exit(main())
