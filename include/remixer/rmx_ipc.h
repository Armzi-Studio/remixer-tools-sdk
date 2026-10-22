/**
 * @file rmx_ipc.h
 * @brief Inter-Process Communication (IPC) protocol, Shared Memory layout,
 *        and Lock-Free SPSC Ring Buffer contracts between DAW and Standalone App.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_IPC_H
#define REMIXER_IPC_H

#include "rmx_types.h"
#include "rmx_audio.h"
#include "rmx_midi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Shared Memory Protocol Constants
 * ------------------------------------------------------------------------- */
#define RMX_SHM_MAGIC           0x524D5853 /* "RMXS" in ASCII */
#define RMX_SHM_PROTOCOL_VER    1
#define RMX_SHM_DEFAULT_NAME    "RemixerTools_AudioBridge_SHM"
#define RMX_SHM_MUTEX_NAME      "RemixerTools_AudioBridge_Lock"

/* Maximum capacities (Must be power of two for atomic masking) */
#define RMX_SHM_AUDIO_RING_FRAMES 16384
#define RMX_SHM_MIDI_QUEUE_SIZE   1024

/* -------------------------------------------------------------------------
 * Transport Synchronization State
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_transport_state {
    double   bpm;                   /**< Beats per minute (e.g. 120.0) */
    double   time_sig_numerator;    /**< Time signature top (e.g. 4) */
    double   time_sig_denominator;  /**< Time signature bottom (e.g. 4) */
    uint64_t current_sample_pos;    /**< Current DAW playhead sample pos */
    uint64_t loop_start_sample;     /**< Loop start boundary */
    uint64_t loop_end_sample;       /**< Loop end boundary */
    uint32_t is_playing;            /**< 1 if DAW transport is rolling */
    uint32_t is_recording;          /**< 1 if DAW is actively recording */
    uint32_t loop_enabled;          /**< 1 if DAW cycle/loop is active */
    uint32_t reserved;
} rmx_transport_state_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Shared Memory Master Header
 * -------------------------------------------------------------------------
 * Layout in shared memory space:
 * [rmx_shm_header_t] (Cache-line aligned)
 * [Audio Ring Buffer: DAW -> Standalone (Stereo/Multichannel)]
 * [Audio Ring Buffer: Standalone -> DAW (Stereo/Multichannel)]
 * [MIDI Ring Buffer: Bidirectional]
 */
#pragma pack(push, 8)
typedef struct rmx_shm_header {
    uint32_t magic;                 /**< Must be RMX_SHM_MAGIC */
    uint32_t protocol_version;      /**< Protocol specification version */
    uint32_t client_pid;            /**< Process ID of the DAW Plugin instance */
    uint32_t host_pid;              /**< Process ID of the Standalone App */

    /* Audio Configuration */
    uint32_t sample_rate;           /**< Negotiated sample rate */
    uint32_t buffer_size_frames;    /**< DAW block size (e.g. 128, 256, 512) */
    uint32_t num_channels;          /**< Number of interleaved audio channels */
    uint32_t ring_capacity_frames;  /**< Size of ring buffer in frames (power of 2) */

    /* Transport State */
    rmx_transport_state_t transport;

    /* Lock-Free Atomic Indices (Audio DAW -> App) */
    volatile uint32_t audio_in_write_idx;
    volatile uint32_t audio_in_read_idx;

    /* Lock-Free Atomic Indices (Audio App -> DAW) */
    volatile uint32_t audio_out_write_idx;
    volatile uint32_t audio_out_read_idx;

    /* Lock-Free Atomic Indices (MIDI Queue) */
    volatile uint32_t midi_write_idx;
    volatile uint32_t midi_read_idx;

    /* Heartbeat & Liveness (Updated by both processes to detect crash/stalls) */
    volatile uint64_t daw_heartbeat_counter;
    volatile uint64_t app_heartbeat_counter;

    /* Reserved for future expansion / 64-byte cache line padding */
    uint8_t  padding[128];
} rmx_shm_header_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_IPC_H */
