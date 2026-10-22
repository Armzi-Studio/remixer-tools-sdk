/**
 * @file rmx_audio.h
 * @brief Audio buffer representation, sample formats, and stem channel layouts.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_AUDIO_H
#define REMIXER_AUDIO_H

#include "rmx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Sample Rates & Limits
 * ------------------------------------------------------------------------- */
#define RMX_SAMPLE_RATE_44100 44100
#define RMX_SAMPLE_RATE_48000 48000
#define RMX_SAMPLE_RATE_88200 88200
#define RMX_SAMPLE_RATE_96000 96000
#define RMX_SAMPLE_RATE_192000 192000

#define RMX_MAX_CHANNELS 32
#define RMX_DEFAULT_BLOCK_SIZE 512

/* -------------------------------------------------------------------------
 * Stem Separation Indices & Channel Layouts
 * ------------------------------------------------------------------------- */
typedef enum rmx_stem_channel {
    RMX_STEM_MASTER       = 0,
    RMX_STEM_VOCALS       = 1,
    RMX_STEM_DRUMS        = 2,
    RMX_STEM_BASS         = 3,
    RMX_STEM_OTHER        = 4,
    RMX_STEM_PIANO        = 5,
    RMX_STEM_GUITAR       = 6,
    RMX_STEM_CUSTOM_BASE  = 16
} rmx_stem_channel_t;

typedef enum rmx_channel_layout {
    RMX_LAYOUT_MONO            = 1,   /* 1 channel */
    RMX_LAYOUT_STEREO          = 2,   /* 2 channels: Left, Right */
    RMX_LAYOUT_4_STEM_STEREO   = 8,   /* 8 channels: 4 Stereo stems (Vocal, Drum, Bass, Other) */
    RMX_LAYOUT_6_STEM_STEREO   = 12,  /* 12 channels: 6 Stereo stems */
    RMX_LAYOUT_SURROUND_5_1    = 6,   /* 5.1 Surround */
    RMX_LAYOUT_SURROUND_7_1    = 8    /* 7.1 Surround */
} rmx_channel_layout_t;

/* -------------------------------------------------------------------------
 * Audio Buffer Descriptor
 * -------------------------------------------------------------------------
 * Planar (non-interleaved) 32-bit floating point audio buffer.
 * Each channel points to an array of num_frames contiguous float samples.
 * Range of normalized audio: [-1.0f, +1.0f]
 */
#pragma pack(push, 8)
typedef struct rmx_audio_buffer {
    float**             channels;         /**< Array of pointers to channel sample data */
    uint32_t            num_channels;     /**< Total number of active channels */
    uint32_t            num_frames;       /**< Number of sample frames per channel */
    uint32_t            sample_rate;      /**< Sample rate in Hz (e.g. 44100, 48000) */
    uint64_t            sample_position;  /**< Global absolute timeline sample position */
    rmx_channel_layout_t layout;          /**< Channel layout flag */
    uint32_t            reserved;         /**< Reserved for 64-bit alignment / future use */
} rmx_audio_buffer_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Audio Stream Properties
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_audio_stream_info {
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t bit_depth;         /**< e.g. 16, 24, 32 */
    uint64_t total_frames;      /**< 0 if stream is infinite / realtime */
    double   duration_seconds;
} rmx_audio_stream_info_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_AUDIO_H */
