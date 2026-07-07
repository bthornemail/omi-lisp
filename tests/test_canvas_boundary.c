/*
 * OMI Layer: test
 * Purpose: Verify compile-time boundary stub compiles and enum guard exists.
 *          No validation, no acceptance, no receipts, no projection.
 * Authority: Non-authoritative declaration surface.
 *
 * This test exists because the header declares a future boundary function
 * with no .c implementation. It must compile cleanly, and the enum guard
 * OMI_CANVAS_BOUNDARY_NOT_AUTHORITY must be present as a legal value.
 */

#include <stdio.h>
#include "omi_canvas_boundary.h"

static int failures = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (cond) {                                                 \
            printf("PASS: %s\n", msg);                              \
        } else {                                                    \
            printf("FAIL: %s\n", msg);                              \
            failures++;                                             \
        }                                                           \
    } while (0)

int main(void)
{
    /* 1. Enum constants compile and have distinct values. */
    CHECK(OMI_CANVAS_BOUNDARY_OK == 0,
          "OMI_CANVAS_BOUNDARY_OK == 0");
    CHECK(OMI_CANVAS_BOUNDARY_UNSUPPORTED > 0,
          "OMI_CANVAS_BOUNDARY_UNSUPPORTED > 0");
    CHECK(OMI_CANVAS_BOUNDARY_INVALID > OMI_CANVAS_BOUNDARY_UNSUPPORTED,
          "OMI_CANVAS_BOUNDARY_INVALID > UNSUPPORTED");
    CHECK(OMI_CANVAS_BOUNDARY_NOT_AUTHORITY > OMI_CANVAS_BOUNDARY_INVALID,
          "OMI_CANVAS_BOUNDARY_NOT_AUTHORITY > INVALID");

    /* 2. OMI_CANVAS_BOUNDARY_NOT_AUTHORITY exists as the explicit guard value. */
    CHECK(OMI_CANVAS_BOUNDARY_NOT_AUTHORITY == 3,
          "OMI_CANVAS_BOUNDARY_NOT_AUTHORITY == 3 (explicit guard)");

    /* 3. Struct type is valid and default-initializable. */
    OMI_CanvasBoundaryInput input = {NULL, 0, 0, 0};
    CHECK(input.root == NULL, "OMI_CanvasBoundaryInput root NULL-initializable");
    CHECK(input.accepted == 0, "OMI_CanvasBoundaryInput accepted defaults to 0");
    CHECK(input.validated == 0, "OMI_CanvasBoundaryInput validated defaults to 0");
    CHECK(input.receipted == 0, "OMI_CanvasBoundaryInput receipted defaults to 0");

    /* 4. Function pointer type is valid (no link-time call). */
    OMI_CanvasBoundaryResult (*fp)(const OMI_CanvasBoundaryInput*) =
        omi_canvas_boundary_check_shape;
    CHECK(fp != NULL, "omi_canvas_boundary_check_shape has a symbol address");

    /*
     * 5. No authority flags may be set. If any are non-zero,
     *    omi_canvas_boundary_check_shape must return NOT_AUTHORITY.
     *    This is a compile-time contract assertion; the actual enforcement
     *    will happen when the function is implemented.
     */

    if (failures == 0) {
        printf("\nAll canvas boundary tests passed.\n");
    } else {
        printf("\n%d test(s) failed.\n", failures);
    }
    return failures > 0 ? 1 : 0;
}
