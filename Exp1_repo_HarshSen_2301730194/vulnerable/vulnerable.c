/* =====================================================================
 * vulnerable.c
 *
 * Experiment 1 - Data Type Conversion: UNSAFE / VULNERABLE VERSION
 *
 * Demonstrates four classic C data-type conversion bugs using plain,
 * unchecked casts and library calls:
 *   1. int   -> short   (truncation)
 *   2. int   -> unsigned int (sign-conversion)
 *   3. float -> int     (overflow / undefined behaviour)
 *   4. string -> numeric (silent failure / overflow with atoi)
 *
 * Each conversion prints the value BEFORE and AFTER, so the unexpected
 * output is visible directly in the console.
 *
 * Build: gcc -Wall -Wextra vulnerable.c -o vulnerable
 * (Intentionally compiled WITHOUT -Wconversion so the bugs are not
 *  flagged at compile time either -- exactly how this mistake ships
 *  in real, insufficiently-warned codebases.)
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void)
{
    printf("================================================================\n");
    printf(" EXPERIMENT 1 - UNSAFE / VULNERABLE DATA TYPE CONVERSIONS\n");
    printf("================================================================\n\n");

    /* -------------------------------------------------------------
     * 1. int -> short  (TRUNCATION)
     * ------------------------------------------------------------- */
    printf("---- [1] int -> short (truncation) ----\n");
    {
        int big_value = 70000;                 /* out of short's range   */
        short truncated = (short)big_value;    /* unchecked narrowing    */

        printf("Before (int)         : %d\n", big_value);
        printf("After  (short, cast) : %d   <-- WRONG, wrapped around\n\n", truncated);
    }

    /* -------------------------------------------------------------
     * 2. signed int -> unsigned int  (SIGN CONVERSION)
     * ------------------------------------------------------------- */
    printf("---- [2] signed int -> unsigned int (sign conversion) ----\n");
    {
        int neg_value = -1;
        unsigned int converted = (unsigned int)neg_value; /* unchecked */

        printf("Before (int, signed)   : %d\n", neg_value);
        printf("After  (unsigned int)  : %u   <-- WRONG, became huge positive\n\n",
               converted);
    }

    /* -------------------------------------------------------------
     * 3. float -> int  (OVERFLOW / UNDEFINED BEHAVIOUR)
     * ------------------------------------------------------------- */
    printf("---- [3] float -> int (overflow) ----\n");
    {
        float huge_float = 1.0e10f;    /* far beyond INT_MAX */
        int converted = (int)huge_float;   /* unchecked, UB in C */

        printf("Before (float)  : %.2f\n", huge_float);
        printf("After  (int)    : %d   <-- WRONG / undefined behaviour\n\n", converted);
    }

    /* -------------------------------------------------------------
     * 4. string -> numeric  (SILENT FAILURE / OVERFLOW)
     * ------------------------------------------------------------- */
    printf("---- [4] string -> numeric (atoi, unchecked) ----\n");
    {
        const char *garbage   = "12abc";            /* partially valid   */
        const char *not_a_num = "hello";             /* not numeric at all */
        const char *too_big   = "99999999999999999"; /* overflows int    */

        int v1 = atoi(garbage);
        int v2 = atoi(not_a_num);
        int v3 = atoi(too_big);

        printf("Before (string) : \"%s\"\n", garbage);
        printf("After  (int)    : %d   <-- silently ignored trailing 'abc'\n\n", v1);

        printf("Before (string) : \"%s\"\n", not_a_num);
        printf("After  (int)    : %d   <-- WRONG, no error reported, looks valid\n\n", v2);

        printf("Before (string) : \"%s\"\n", too_big);
        printf("After  (int)    : %d   <-- WRONG, overflowed silently\n\n", v3);
    }

    printf("================================================================\n");
    printf(" All conversions above completed WITHOUT any error checking.\n");
    printf(" Every '<-- WRONG' line is a bug a real program could ship\n");
    printf(" with no compiler error and no runtime crash.\n");
    printf("================================================================\n");

    return 0;
}
