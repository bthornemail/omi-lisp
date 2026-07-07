/*
 * OMI Layer: adapter
 * Purpose: Compile-time boundary stub for future omi-canvas handoff.
 *          Declares the future boundary shape without making it executable.
 *          No omi-canvas import. No tetragrammatron import.
 *          No validation, no acceptance, no receipts, no projection.
 * Authority: Non-authoritative declaration surface only.
 *
 * This is a header-only boundary declaration. No .c implementation exists.
 * The first real integration replaces this stub with linkable code that
 * calls omi-canvas. Until then, consumers see only the port shape.
 *
 * OMI_CANVAS_BOUNDARY_NOT_AUTHORITY is the explicit guard value:
 * no authority crosses this boundary until the integration gate opens.
 */

#ifndef OMI_CANVAS_BOUNDARY_H
#define OMI_CANVAS_BOUNDARY_H

#include "omi_adapter_contract.h"

typedef enum {
    OMI_CANVAS_BOUNDARY_OK = 0,
    OMI_CANVAS_BOUNDARY_UNSUPPORTED,
    OMI_CANVAS_BOUNDARY_INVALID,
    OMI_CANVAS_BOUNDARY_NOT_AUTHORITY
} OMI_CanvasBoundaryResult;

typedef struct {
    const OMI_AdapterNode* root;
    int accepted;
    int validated;
    int receipted;
} OMI_CanvasBoundaryInput;

OMI_CanvasBoundaryResult omi_canvas_boundary_check_shape(
    const OMI_CanvasBoundaryInput* input
);

#endif /* OMI_CANVAS_BOUNDARY_H */
