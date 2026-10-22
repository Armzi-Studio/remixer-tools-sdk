/**
 * @file rmx_license.h
 * @brief Licensing, Hardware ID (HWID) binding, and Dynamic Anti-Tamper protocol.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_LICENSE_H
#define REMIXER_LICENSE_H

#include "rmx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * License Tiers & Feature Bitmasks
 * ------------------------------------------------------------------------- */
typedef enum rmx_license_tier {
    RMX_TIER_FREE_COMMUNITY = 0,
    RMX_TIER_CREATOR        = 1,
    RMX_TIER_PRO            = 2,
    RMX_TIER_STUDIO         = 3,
    RMX_TIER_ENTERPRISE     = 4
} rmx_license_tier_t;

typedef enum rmx_feature_flags {
    RMX_FEAT_BASIC_PLAYBACK   = (1 << 0),
    RMX_FEAT_4_STEM_SEPARATION= (1 << 1),
    RMX_FEAT_6_STEM_SEPARATION= (1 << 2),
    RMX_FEAT_AUDIO_TO_MIDI    = (1 << 3),
    RMX_FEAT_VST_BRIDGE       = (1 << 4),
    RMX_FEAT_BATCH_PROCESSING = (1 << 5),
    RMX_FEAT_CLOUD_SYNC       = (1 << 6),
    RMX_FEAT_ALL              = 0xFFFFFFFF
} rmx_feature_flags_t;

/* -------------------------------------------------------------------------
 * Hardware Identifier (HWID) Hash
 * -------------------------------------------------------------------------
 * SHA-256 Digest of combined system identifiers:
 * Motherboard UUID + CPU Microcode ID + Primary Storage Serial + Primary MAC
 */
#define RMX_HWID_BYTES 32

#pragma pack(push, 8)
typedef struct rmx_hwid {
    uint8_t  digest[RMX_HWID_BYTES]; /**< 256-bit unique cryptographic machine hash */
    char     platform_name[32];      /**< e.g. "Windows 11 x64", "macOS Sonoma ARM64" */
} rmx_hwid_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * License Verification Token Data
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_license_info {
    char                licensee_id[64];     /**< User Account UUID */
    char                license_key[64];     /**< Formatted product key */
    rmx_license_tier_t  tier;                /**< Subscribed license tier */
    uint32_t            feature_mask;        /**< Bitwise OR of rmx_feature_flags_t */
    uint64_t            issued_timestamp;    /**< Unix epoch timestamp */
    uint64_t            expires_timestamp;   /**< 0 for perpetual license */
    uint32_t            grace_period_sec;    /**< Offline tolerance (e.g. 7 days in seconds) */
    uint32_t            is_activated;        /**< 1 if successfully activated */
} rmx_license_info_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Dynamic Anti-Tamper Heartbeat Challenge
 * -------------------------------------------------------------------------
 * Sent periodically (every 15-30 minutes) between Engine and Backend to
 * prevent memory dumping, dynamic debugging, and offline license spoofing.
 */
#pragma pack(push, 8)
typedef struct rmx_heartbeat_challenge {
    uint64_t nonce;                          /**< Random cryptographic nonce */
    uint64_t timestamp;                      /**< Current client timestamp */
    uint8_t  client_hwid[RMX_HWID_BYTES];    /**< Machine fingerprint */
} rmx_heartbeat_challenge_t;

typedef struct rmx_heartbeat_response {
    uint64_t nonce_ack;                      /**< nonce + 1 signed by server */
    uint64_t valid_until;                    /**< Expiry timestamp of this heartbeat session */
    uint8_t  signature[64];                  /**< Ed25519 digital signature of payload */
} rmx_heartbeat_response_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_LICENSE_H */
