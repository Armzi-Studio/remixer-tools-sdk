/**
 * @file rmx_types.h
 * @brief Core primitive types, error codes, and platform macros for Remixer Tools SDK.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_TYPES_H
#define REMIXER_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Platform / Compiler Export Macros
 * ------------------------------------------------------------------------- */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define RMX_PLATFORM_WINDOWS 1
    #ifdef RMX_BUILDING_DLL
        #define RMX_API __declspec(dllexport)
    #else
        #define RMX_API __declspec(dllimport)
    #endif
    #define RMX_CALL __cdecl
#elif defined(__APPLE__) || defined(__MACH__)
    #define RMX_PLATFORM_APPLE 1
    #define RMX_API __attribute__((visibility("default")))
    #define RMX_CALL
#elif defined(__linux__) || defined(__unix__)
    #define RMX_PLATFORM_LINUX 1
    #define RMX_API __attribute__((visibility("default")))
    #define RMX_CALL
#else
    #define RMX_API
    #define RMX_CALL
#endif

#if defined(_MSC_VER)
    #define RMX_INLINE __forceinline
    #define RMX_ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
    #define RMX_INLINE static inline __attribute__((always_inline))
    #define RMX_ALIGNED(x) __attribute__((aligned(x)))
#else
    #define RMX_INLINE static inline
    #define RMX_ALIGNED(x)
#endif

/* -------------------------------------------------------------------------
 * Versioning
 * ------------------------------------------------------------------------- */
#define RMX_VERSION_MAJOR 1
#define RMX_VERSION_MINOR 0
#define RMX_VERSION_PATCH 0

#define RMX_MAKE_VERSION(major, minor, patch) \
    (((uint32_t)(major) << 16) | ((uint32_t)(minor) << 8) | ((uint32_t)(patch)))

#define RMX_CURRENT_VERSION RMX_MAKE_VERSION(RMX_VERSION_MAJOR, RMX_VERSION_MINOR, RMX_VERSION_PATCH)

typedef uint32_t rmx_version_t;

/* -------------------------------------------------------------------------
 * Return Status & Error Codes
 * ------------------------------------------------------------------------- */
typedef enum rmx_status {
    /* Success */
    RMX_OK                      = 0,

    /* Generic Errors (-1 to -49) */
    RMX_ERR_GENERIC             = -1,
    RMX_ERR_INVALID_ARGUMENT    = -2,
    RMX_ERR_OUT_OF_MEMORY       = -3,
    RMX_ERR_NOT_INITIALIZED     = -4,
    RMX_ERR_ALREADY_INITIALIZED = -5,
    RMX_ERR_NULL_POINTER        = -6,
    RMX_ERR_UNSUPPORTED         = -7,
    RMX_ERR_NOT_FOUND           = -8,
    RMX_ERR_TIMEOUT             = -9,

    /* Buffer & Audio Streaming Errors (-50 to -99) */
    RMX_ERR_BUFFER_OVERFLOW     = -50,
    RMX_ERR_BUFFER_UNDERFLOW    = -51,
    RMX_ERR_SAMPLE_RATE_MISMATCH= -52,
    RMX_ERR_CHANNEL_MISMATCH    = -53,
    RMX_ERR_IPC_DISCONNECTED    = -54,

    /* Security, Licensing & Anti-Tamper Errors (-100 to -199) */
    RMX_ERR_LICENSE_INVALID     = -100,
    RMX_ERR_LICENSE_EXPIRED     = -101,
    RMX_ERR_LICENSE_SEAT_LIMIT  = -102,
    RMX_ERR_HWID_MISMATCH       = -103,
    RMX_ERR_SIGNATURE_FAILED    = -104,
    RMX_ERR_TAMPER_DETECTED     = -105,
    RMX_ERR_DECRYPTION_FAILED   = -106,
    RMX_ERR_MODEL_CORRUPTED     = -107
} rmx_status_t;

/* Helper to check status success */
RMX_INLINE bool rmx_is_ok(rmx_status_t status) {
    return status == RMX_OK;
}

/* Opaque pointer handle representation */
typedef void* rmx_handle_t;

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_TYPES_H */
