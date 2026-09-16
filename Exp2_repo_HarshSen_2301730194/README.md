# Experiment 2 — Compiler Hardening Flags and UBSan/ASan Diagnostics

**Name:** Harsh Sen
**Roll No.:** 2301730194

Compiles the type-conversion program from Experiment 1 (`vulnerable.c`)
under four configurations and records what each one catches:

1. No flags
2. `-Wall -Wextra`
3. `-fsanitize=undefined` (UBSan)
4. `-fsanitize=address` (ASan)

## Structure
- `src/vulnerable.c` — the unsafe type-conversion program under test (from Exp 1).
- `outputs/annotated_1_noflags.txt`
- `outputs/annotated_2_wall_wextra.txt`
- `outputs/annotated_3_ubsan.txt`
- `outputs/annotated_4_asan.txt`
- `outputs/v*_runtime.txt` — raw captured runtime output per configuration.

## Commands used

```bash
gcc src/vulnerable.c -o v1_noflags
gcc -Wall -Wextra src/vulnerable.c -o v2_wall
gcc -g -fsanitize=undefined src/vulnerable.c -o v3_ubsan
gcc -g -fsanitize=address src/vulnerable.c -o v4_asan
```

## Result (summary)

| Config | Compiler warnings | Runtime errors caught | What it missed |
|---|---|---|---|
| No flags | none | none | everything |
| `-Wall -Wextra` | none (casts are explicit) | none | all 4 conversions |
| UBSan | none | none by default; `float-cast-overflow` catches the float→int bug only when enabled explicitly | 3 well-defined-but-wrong conversions |
| ASan | none | none | all 4 (no memory-safety violation exists in this program) |

Full analysis, verified compiler/runtime output, and explanation of
*why* each configuration behaved this way is in the PDF lab report.
