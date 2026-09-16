/* =====================================================================
 * safe.c
 *
 * Experiment 1 - Data Type Conversion: SAFE VERSION
 *
 * Re-implements the same four conversions as vulnerable.c, but every
 * conversion goes through a validating helper function that checks the
 * value's range BEFORE converting, and rejects out-of-range or invalid
 * input with a clear error message instead of silently producing a
 * wrong result.
 *
 *   1. int    -> short       : safe_int_to_short()
 *   2. int    -> unsigned int: safe_int_to_uint()
 *   3. float  -> int         : safe_float_to_int()
 *   4. string -> numeric     : safe_str_to_int()
 *
 * Build: gcc -Wall -Wextra -Wconversion -Wsign-conversion \
 *            -fstack-protector-strong -D_FORTIFY_SOURCE=2 safe.c -o safe
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <math.h>

typedef enum {
    OK = 0,
    ERR_OVERFLOW,
    ERR_UNDERFLOW,
    ERR_NEGATIVE,
    ERR_INVALID_INPUT,
    ERR_NOT_FINITE
} status_t;

static const char *status_msg(status_t s)
{
    switch (s) {
        case OK:               return "OK";
        case ERR_OVERFLOW:     return "ERROR: value too large for destination type";
        case ERR_UNDERFLOW:    return "ERROR: value too small (negative) for destination type";
        case ERR_NEGATIVE:     return "ERROR: negative value cannot become unsigned";
        case ERR_INVALID_INPUT:return "ERROR: input is not a valid number";
        case ERR_NOT_FINITE:   return "ERROR: value is NaN/Infinity, cannot convert";
        default:               return "ERROR: unknown";
    }
}

/* ---- 1. int -> short, range checked -------------------------------- */
static status_t safe_int_to_short(int in, short *out)
{
    if (in > SHRT_MAX) return ERR_OVERFLOW;
    if (in < SHRT_MIN) return ERR_UNDERFLOW;
    *out = (short)in;
    return OK;
}

/* ---- 2. int -> unsigned int, rejects negatives ---------------------- */
static status_t safe_int_to_uint(int in, unsigned int *out)
{
    if (in < 0) return ERR_NEGATIVE;
    *out = (unsigned int)in;
    return OK;
}

/* ---- 3. float -> int, range + finiteness checked -------------------- */
static status_t safe_float_to_int(float in, int *out)
{
    if (isnan(in) || isinf(in)) return ERR_NOT_FINITE;
    if (in > (float)INT_MAX)    return ERR_OVERFLOW;
    if (in < (float)INT_MIN)    return ERR_UNDERFLOW;
    *out = (int)in;
    return OK;
}

/* ---- 4. string -> int, fully validated with strtol ------------------ */
static status_t safe_str_to_int(const char *in, int *out)
{
    if (in == NULL || *in == '\0') return ERR_INVALID_INPUT;

    char *endptr = NULL;
    errno = 0;
    long val = strtol(in, &endptr, 10);

    /* No digits were consumed at all -> not a number */
    if (endptr == in) return ERR_INVALID_INPUT;

    /* Trailing garbage after the number, e.g. "12abc" -> reject */
    if (*endptr != '\0') return ERR_INVALID_INPUT;

    /* strtol signalled overflow/underflow via errno */
    if (errno == ERANGE) {
        return (val == LONG_MAX) ? ERR_OVERFLOW : ERR_UNDERFLOW;
    }

    /* Value fits in long but might still not fit in int */
    if (val > INT_MAX) return ERR_OVERFLOW;
    if (val < INT_MIN) return ERR_UNDERFLOW;

    *out = (int)val;
    return OK;
}

int main(void)
{
    printf("================================================================\n");
    printf(" EXPERIMENT 1 - SAFE DATA TYPE CONVERSIONS (range-checked)\n");
    printf("================================================================\n\n");

    /* -------------------------------------------------------------
     * 1. int -> short
     * ------------------------------------------------------------- */
    printf("---- [1] int -> short (safe) ----\n");
    {
        int values[] = { 100, 32767, 70000, -40000 };
        for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
            short result = 0;
            status_t st = safe_int_to_short(values[i], &result);
            printf("Before (int) : %d\n", values[i]);
            if (st == OK)
                printf("After  (short): %d\n\n", result);
            else
                printf("After  (short): REJECTED - %s\n\n", status_msg(st));
        }
    }

    /* -------------------------------------------------------------
     * 2. signed int -> unsigned int
     * ------------------------------------------------------------- */
    printf("---- [2] signed int -> unsigned int (safe) ----\n");
    {
        int values[] = { 42, -1, -100 };
        for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
            unsigned int result = 0;
            status_t st = safe_int_to_uint(values[i], &result);
            printf("Before (int, signed) : %d\n", values[i]);
            if (st == OK)
                printf("After  (unsigned int): %u\n\n", result);
            else
                printf("After  (unsigned int): REJECTED - %s\n\n", status_msg(st));
        }
    }

    /* -------------------------------------------------------------
     * 3. float -> int
     * ------------------------------------------------------------- */
    printf("---- [3] float -> int (safe) ----\n");
    {
        float values[] = { 3.99f, 1.0e10f, -1.0e10f };
        for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
            int result = 0;
            status_t st = safe_float_to_int(values[i], &result);
            printf("Before (float): %.2f\n", values[i]);
            if (st == OK)
                printf("After  (int)  : %d\n\n", result);
            else
                printf("After  (int)  : REJECTED - %s\n\n", status_msg(st));
        }
    }

    /* -------------------------------------------------------------
     * 4. string -> numeric
     * ------------------------------------------------------------- */
    printf("---- [4] string -> numeric (safe, strtol-based) ----\n");
    {
        const char *values[] = { "12345", "12abc", "hello", "99999999999999999" };
        for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
            int result = 0;
            status_t st = safe_str_to_int(values[i], &result);
            printf("Before (string): \"%s\"\n", values[i]);
            if (st == OK)
                printf("After  (int)   : %d\n\n", result);
            else
                printf("After  (int)   : REJECTED - %s\n\n", status_msg(st));
        }
    }

    printf("================================================================\n");
    printf(" Every out-of-range or invalid input above was rejected with\n");
    printf(" a clear error message instead of silently producing a wrong\n");
    printf(" value.\n");
    printf("================================================================\n");

    return 0;
}
