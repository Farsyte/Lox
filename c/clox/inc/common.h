#pragma once

#include "error_log.h"
#include "types.h"

#include <stddef.h>
#include <stdint.h>

/** @file common.h
 * @brief Import APIs that all modules need
 *
 * This file includes a number of headers that will
 * be needed by most, if not all, source files.
 *
 * It also is the central point for turing on and off
 * macros that control optional behaviors.
 */

/** Enable (or disable) code printing */
#define DEBUG_PRINT_CODE

/** Enable (or disable) execution tracing */
#define DEBUG_TRACE_EXECUTION

/** Enable (or disable) stressing the GC */
#define DEBUG_STRESS_GC

/** Number of distinct uint8_t values */
#define UINT8_COUNT (UINT8_MAX + 1)
