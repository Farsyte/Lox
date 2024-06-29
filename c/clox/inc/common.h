#pragma once

#include "error_log.h"
#include "types.h"

#include <stdbool.h>
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

/** Enable (or disable) execution tracing */
#define DEBUG_TRACE_EXECUTION
