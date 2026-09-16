# Secure C Programming Lab — Harsh Sen (2301730194)

Two experiments on integer/data-type conversion bugs in C.

## Exp 1 — Data Type Conversion: Unsafe vs Safe
`exp1/vulnerable/vulnerable.c` — unchecked `int→short`, `int→unsigned`,
`float→int`, `atoi()` conversions; shows truncation, sign-conversion,
and overflow bugs with wrong-but-silent output.

`exp1/safe/safe.c` — same conversions through range-checked helpers
that reject bad input with a clear error message.

```bash
gcc -Wall -Wextra exp1/vulnerable/vulnerable.c -o vulnerable && ./vulnerable
gcc -Wall -Wextra -Wconversion -Wsign-conversion -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 exp1/safe/safe.c -o safe -lm && ./safe
```

Report: `Exp1_Data_Type_Conversion_Report.pdf`

## Exp 2 — Compiler Hardening Flags and UBSan/ASan Diagnostics
Compiles Exp 1's `vulnerable.c` under 4 configs — no flags, `-Wall
-Wextra`, `-fsanitize=undefined`, `-fsanitize=address` — and records
what each one catches (and why the "silent" results happen: explicit
casts suppress `-Wconversion`; GCC's `undefined` group excludes
`float-cast-overflow` unless named explicitly; ASan targets memory
safety, not conversion logic).

```bash
gcc src/vulnerable.c -o v1                              # no flags
gcc -Wall -Wextra src/vulnerable.c -o v2
gcc -g -fsanitize=undefined src/vulnerable.c -o v3
gcc -g -fsanitize=address src/vulnerable.c -o v4
```

Annotated outputs: `exp2/outputs/annotated_*.txt`
Report: `Exp2_Compiler_Hardening_UBSan_ASan_Report.pdf`

## Result
Neither compiler warnings nor sanitizers reliably catch a conversion
bug once it's hidden behind an explicit cast or falls outside the
sanitizer's scope. Explicit range-checking before conversion (Exp 1's
`safe.c`) is the only approach tested that actually prevents the bug
rather than just trying to detect it.
