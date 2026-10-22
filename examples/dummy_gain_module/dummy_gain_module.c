/**
 * @file dummy_gain_module.c
 * @brief Reference implementation of an audio gain module using the Remixer C-ABI.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#define RMX_BUILDING_DLL 1
#include "remixer/remixer.h"
#include <stdlib.h>
#include <string.h>

typedef struct dummy_gain_instance {
    float gain;
    uint32_t sample_rate;
} dummy_gain_instance_t;

static rmx_status_t RMX_CALL dummy_init(rmx_handle_t* instance, const char* config_json) {
    (void)config_json;
    if (!instance) return RMX_ERR_NULL_POINTER;

    dummy_gain_instance_t* state = (dummy_gain_instance_t*)calloc(1, sizeof(dummy_gain_instance_t));
    if (!state) return RMX_ERR_OUT_OF_MEMORY;

    state->gain = 1.0f;
    state->sample_rate = RMX_SAMPLE_RATE_48000;
    *instance = (rmx_handle_t)state;
    return RMX_OK;
}

static rmx_status_t RMX_CALL dummy_prepare_to_play(rmx_handle_t instance, uint32_t sample_rate, uint32_t max_block_size) {
    (void)max_block_size;
    dummy_gain_instance_t* state = (dummy_gain_instance_t*)instance;
    if (!state) return RMX_ERR_NOT_INITIALIZED;

    state->sample_rate = sample_rate;
    return RMX_OK;
}

static rmx_status_t RMX_CALL dummy_process_audio(rmx_handle_t instance, const rmx_audio_buffer_t* in_buf, rmx_audio_buffer_t* out_buf) {
    dummy_gain_instance_t* state = (dummy_gain_instance_t*)instance;
    if (!state) return RMX_ERR_NOT_INITIALIZED;
    if (!in_buf || !out_buf) return RMX_ERR_NULL_POINTER;

    uint32_t num_ch = in_buf->num_channels < out_buf->num_channels ? in_buf->num_channels : out_buf->num_channels;
    uint32_t frames = in_buf->num_frames < out_buf->num_frames ? in_buf->num_frames : out_buf->num_frames;
    float gain = state->gain;

    for (uint32_t ch = 0; ch < num_ch; ++ch) {
        const float* src = in_buf->channels[ch];
        float* dst = out_buf->channels[ch];
        if (!src || !dst) continue;

        for (uint32_t i = 0; i < frames; ++i) {
            dst[i] = src[i] * gain;
        }
    }

    return RMX_OK;
}

static rmx_status_t RMX_CALL dummy_process_midi(rmx_handle_t instance, const rmx_midi_buffer_t* in_midi, rmx_midi_buffer_t* out_midi) {
    (void)instance;
    (void)in_midi;
    (void)out_midi;
    return RMX_OK;
}

static rmx_status_t RMX_CALL dummy_set_parameter(rmx_handle_t instance, uint32_t param_id, float value) {
    dummy_gain_instance_t* state = (dummy_gain_instance_t*)instance;
    if (!state) return RMX_ERR_NOT_INITIALIZED;

    if (param_id == 0) { /* Gain */
        state->gain = value;
        return RMX_OK;
    }
    return RMX_ERR_INVALID_ARGUMENT;
}

static rmx_status_t RMX_CALL dummy_get_parameter(rmx_handle_t instance, uint32_t param_id, float* out_value) {
    dummy_gain_instance_t* state = (dummy_gain_instance_t*)instance;
    if (!state) return RMX_ERR_NOT_INITIALIZED;
    if (!out_value) return RMX_ERR_NULL_POINTER;

    if (param_id == 0) {
        *out_value = state->gain;
        return RMX_OK;
    }
    return RMX_ERR_INVALID_ARGUMENT;
}

static rmx_status_t RMX_CALL dummy_reset(rmx_handle_t instance) {
    (void)instance;
    return RMX_OK;
}

static rmx_status_t RMX_CALL dummy_destroy(rmx_handle_t instance) {
    dummy_gain_instance_t* state = (dummy_gain_instance_t*)instance;
    if (state) {
        free(state);
    }
    return RMX_OK;
}

static const rmx_module_interface_t g_dummy_interface = {
    dummy_init,
    dummy_prepare_to_play,
    dummy_process_audio,
    dummy_process_midi,
    dummy_set_parameter,
    dummy_get_parameter,
    dummy_reset,
    dummy_destroy
};

RMX_API rmx_status_t RMX_CALL rmx_get_module_desc(rmx_module_desc_t* out_desc) {
    if (!out_desc) return RMX_ERR_NULL_POINTER;

    out_desc->api_version = RMX_CURRENT_VERSION;
    strncpy(out_desc->module_id, "com.armzistudio.dummy_gain", sizeof(out_desc->module_id) - 1);
    strncpy(out_desc->name, "Reference Gain Module", sizeof(out_desc->name) - 1);
    strncpy(out_desc->vendor, "Armzi Studio", sizeof(out_desc->vendor) - 1);
    strncpy(out_desc->version_str, "1.0.0", sizeof(out_desc->version_str) - 1);
    strncpy(out_desc->description, "Reference C-ABI module for testing SDK integration", sizeof(out_desc->description) - 1);
    out_desc->category = RMX_CAT_EFFECT;
    out_desc->flags = 0;
    return RMX_OK;
}

RMX_API rmx_status_t RMX_CALL rmx_create_module(const rmx_module_interface_t** out_interface, rmx_handle_t* out_instance) {
    if (!out_interface || !out_instance) return RMX_ERR_NULL_POINTER;

    *out_interface = &g_dummy_interface;
    return dummy_init(out_instance, NULL);
}
