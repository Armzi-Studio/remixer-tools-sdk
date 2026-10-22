/**
 * @file rmx_midi.h
 * @brief MIDI event types, pitch detection representations, and note quantizer formats.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_MIDI_H
#define REMIXER_MIDI_H

#include "rmx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * MIDI Status Bytes (Standard MIDI 1.0)
 * ------------------------------------------------------------------------- */
typedef enum rmx_midi_status {
    RMX_MIDI_NOTE_OFF         = 0x80,
    RMX_MIDI_NOTE_ON          = 0x90,
    RMX_MIDI_POLY_PRESSURE    = 0xA0,
    RMX_MIDI_CONTROL_CHANGE   = 0xB0,
    RMX_MIDI_PROGRAM_CHANGE   = 0xC0,
    RMX_MIDI_CHANNEL_PRESSURE = 0xD0,
    RMX_MIDI_PITCH_BEND       = 0xE0
} rmx_midi_status_t;

/* -------------------------------------------------------------------------
 * MIDI Event Structure (Real-Time Safe)
 * ------------------------------------------------------------------------- */
#pragma pack(push, 4)
typedef struct rmx_midi_event {
    uint32_t delta_frames; /**< Frame offset within the current audio block */
    uint8_t  status;       /**< Status byte (e.g. 0x90 for Note On) */
    uint8_t  channel;      /**< 0 - 15 (Channels 1 - 16) */
    uint8_t  data1;        /**< Note number (0-127) or CC index */
    uint8_t  data2;        /**< Velocity (0-127) or CC value */
} rmx_midi_event_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * High-Precision Pitch Event (Used by remixer-audio2midi)
 * -------------------------------------------------------------------------
 * Stores microtonal pitch detection data before or alongside standard quantization.
 */
#pragma pack(push, 8)
typedef struct rmx_pitch_event {
    uint64_t sample_offset;    /**< Absolute sample offset in track */
    double   frequency_hz;     /**< Detected fundamental frequency in Hz */
    float    confidence;       /**< Detection confidence score [0.0f, 1.0f] */
    float    amplitude;        /**< RMS amplitude or loudness [0.0f, 1.0f] */
    uint8_t  quantized_note;   /**< Closest MIDI note (0-127) */
    int8_t   cents_offset;     /**< Microtonal deviation (-50 to +50 cents) */
    uint8_t  is_voiced;        /**< 1 if voiced/harmonic, 0 if unvoiced/noise */
    uint8_t  reserved;
} rmx_pitch_event_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * MIDI Event Buffer (Block-level Event Container)
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_midi_buffer {
    rmx_midi_event_t* events;     /**< Array of MIDI events */
    uint32_t          num_events; /**< Current active events in this block */
    uint32_t          capacity;   /**< Total allocated capacity */
} rmx_midi_buffer_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_MIDI_H */
