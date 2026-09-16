# Experiment 1 — Data Type Conversion: Unsafe Behaviour and Safe Implementation

**Name:** Harsh Sen
**Roll No.:** 2301730194

Demonstrates unsafe vs. safe C data type conversions:
- `int -> short` (truncation)
- `int -> unsigned int` (sign conversion)
- `float -> int` (overflow)
- `string -> numeric` (invalid input / overflow)

## Structure
- `vulnerable/vulnerable.c` — unchecked casts/`atoi`, shows the unsafe/unexpected output.
- `safe/safe.c` — same conversions through range-checked helper functions that reject bad input with a clear error message.

## Build & run

```bash
# Vulnerable version
gcc -Wall -Wextra vulnerable/vulnerable.c -o vulnerable/vulnerable
./vulnerable/vulnerable

# Safe version
gcc -Wall -Wextra -Wconversion -Wsign-conversion \
    -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
    safe/safe.c -o safe/safe -lm
./safe/safe
```

## Result

The vulnerable version compiles and runs without any crash, but silently
produces wrong values (truncated, sign-flipped, overflowed, or garbage
parsed from invalid strings). The safe version performs a range/validity
check before every conversion and rejects out-of-range or invalid input
with a clear error message instead of producing an incorrect result.
