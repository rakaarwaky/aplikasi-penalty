#!/usr/bin/env python3
"""
mutation_test.py — Mutation testing gate untuk aplikasi-penalty.

Cara kerja:
  Untuk tiap mutan: backup file sumber -> patch (1 token) -> rebuild test
  binary secara incremental -> jalankan ./run_tests. Jika test GAGAL, mutan
  dianggap "killed" (gate menangkap bug). Jika test tetap hijau, mutan
  "survived" (gate butuh test tambahan). Lalu restore file & binary normal.

Metrik:  mutation score = killed / total  (semakin tinggi, gate makin kuat)

UKURAN KECEPATAN: rebuild incremental per mutan (~2-4s), tanpa `make clean`.
"""
import subprocess, os, sys, shutil, time

ROOT = "/home/raka/Study/PengantarCoding/aplikasi-penalty"
os.chdir(ROOT)

SRC = "src"

# (file_relatif, old_substring, new_substring, deskripsi)
MUTANTS = [
    # --- boundary / relational flip ---
    ("scoring/capabilities_scoring_zone_validator.c",
     "zone.value < MIN_ZONE", "zone.value <= MIN_ZONE",
     "zona: terima batas bawah (0 jadi valid sebagai invalid leak)"),
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
    # --- relational logic flip ---
    ("registration/capabilities_registration_validator.c",
     "else if (c != ' ') return REG_NAME_INVALID_CHAR;",
     "else if (c == ' ') return REG_NAME_INVALID_CHAR;",
     "nama: karakter spasi dianggap invalid (logika terbalik)"),
    ("sanitizer/capabilities_sanitizer_validator.c",
     "if (*endptr == input) return SANITIZE_ERROR_INVALID_CHARS;",
     "if (*endptr != input) return SANITIZE_ERROR_INVALID_CHARS;",
     "sanitizer: empty string tidak lagi ditolak"),
    ("storage/infrastructure_storage_adapter.c",
     "if (file == NULL) return ST_ERROR_PERMISSION;",
     "if (file != NULL) return ST_ERROR_PERMISSION;",
     "storage save: file gagal dibuka malah dianggap OK"),
    ("export/infrastructure_export_adapter.c",
     "if (file == NULL) return EXP_ERROR_PERMISSION;",
     "if (file != NULL) return EXP_ERROR_PERMISSION;",
     "export: file gagal dibuka malah dianggap OK"),
    ("recap/capabilities_recap_formatter.c",
     "if (state == NULL || details == NULL) return RC_NOT_READY;",
     "if (state == NULL || details != NULL) return RC_NOT_READY;",
     "recap: details NULL tidak lagi ditolak"),
    # --- return-code swap (salah return) ---
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
     "if (written != 1 || fclose(file) != 0) return ST_ERROR_CORRUPT;",
     "if (written != 1 || fclose(file) == 0) return ST_ERROR_CORRUPT;",
     "storage: fclose error diabaikan (false-success)"),
]

def run_cmd(cmd):
    return subprocess.run(cmd, shell=True, capture_output=True, text=True)

def build_and_test():
    # incremental build (MAKE akan recompile file yang berubah saja)
    b = run_cmd("make test >/dev/null 2>&1")
    if b.returncode != 0:
        return ("BUILD_FAIL", b.returncode)
    r = run_cmd("./run_tests >/dev/null 2>&1")
    return ("PASS" if r.returncode == 0 else "FAIL", r.returncode)

def main():
    print("=== MUTATION TESTING GATE ===")
    # pastikan baseline hijau
    print("[baseline] build + test normal...", end=" ")
    status, _ = build_and_test()
    if status != "PASS":
        print(f"GAGAL baseline ({status}) — perbaiki dulu sebelum mutation test")
        sys.exit(1)
    print("hijau.\n")

    killed, survived, buildfail = [], [], []
    for i, (rel, old, new, desc) in enumerate(MUTANTS, 1):
        path = os.path.join(SRC, rel)
        bak = path + ".mutbak"
        shutil.copy(path, bak)
        try:
            s = open(path).read()
            if old not in s:
                print(f"[{i}/{len(MUTANTS)}] SKIP (pola tak ditemukan): {desc}")
                os.remove(bak)
                continue
            s = s.replace(old, new, 1)
            open(path, "w").write(s)
            status, _ = build_and_test()
        finally:
            shutil.move(bak, path)  # restore segera
            # rebuild binary normal agar mutan berikutnya mulai dari state bersih
            run_cmd("make test >/dev/null 2>&1")
        tag = {"PASS": "SURVIVED", "FAIL": "KILLED",
               "BUILD_FAIL": "BUILD_FAIL"}[status]
        line = f"[{i}/{len(MUTANTS)}] {tag:10} | {desc}"
        print(line)
        if status == "FAIL": killed.append(desc)
        elif status == "PASS": survived.append(desc)
        else: buildfail.append(desc)

    total = len(MUTANTS)
    score = len(killed) / total * 100 if total else 0
    print("\n=== HASIL ===")
    print(f"Total mutan    : {total}")
    print(f"Killed (gate nangkep) : {len(killed)}")
    print(f"Survived (lolos)       : {len(survived)}")
    print(f"Build fail             : {len(buildfail)}")
    print(f"MUTATION SCORE : {score:.1f}%")
    if survived:
        print("\nMutan yang SELAMAT (butuh test tambahan):")
        for d in survived: print(f"  - {d}")
    # pastikan repo kembali hijau
    run_cmd("make test >/dev/null 2>&1")
    return 0 if score >= 80 else 1

if __name__ == "__main__":
    sys.exit(main())
