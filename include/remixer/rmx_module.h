/**
 * @file rmx_module.h
 * @brief Dynamic Module Plugin Interface, Lifecycle vtable, and Parameter descriptors.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_MODULE_H
#define REMIXER_MODULE_H

#include "rmx_types.h"
#include "rmx_audio.h"
#include "rmx_midi.h"
#include "rmx_gui.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Module Feature & Capability Flags
 * ------------------------------------------------------------------------- */
#define RMX_MOD_FLAG_NONE          0
#define RMX_MOD_FLAG_HAS_CUSTOM_UI (1 << 0) /**< Module provides custom GUI via get_ui_interface */
#define RMX_MOD_FLAG_REALTIME_SAFE (1 << 1) /**< DSP processing is lock-free and allocation-free */

/* -------------------------------------------------------------------------
 * Module Categories
 * ------------------------------------------------------------------------- */
typedef enum rmx_module_category {
    RMX_CAT_UNKNOWN         = 0,
    RMX_CAT_CORE_DSP        = 1,
    RMX_CAT_STEM_SEPARATOR  = 2,
    RMX_CAT_AUDIO_TO_MIDI   = 3,
    RMX_CAT_DOWNLOADER      = 4,
    RMX_CAT_EFFECT          = 5,
    RMX_CAT_SYNTHESIZER     = 6,
    RMX_CAT_ANALYZER        = 7
} rmx_module_category_t;

/* -------------------------------------------------------------------------
 * Module Descriptor (Static Metadata)
 * ------------------------------------------------------------------------- */
#define RMX_MAX_NAME_LEN 64
#define RMX_MAX_DESC_LEN 256

#pragma pack(push, 8)
typedef struct rmx_module_desc {
    rmx_version_t         api_version;              /**< RMX_CURRENT_VERSION */
    char                  module_id[64];            /**< e.g. "com.armzistudio.demucs_v4" */
    char                  name[RMX_MAX_NAME_LEN];   /**< Display Name */
    char                  vendor[RMX_MAX_NAME_LEN]; /**< e.g. "Armzi Studio" */
    char                  version_str[32];          /**< e.g. "1.0.0" */
    char                  description[RMX_MAX_DESC_LEN];
    rmx_module_category_t category;
    uint32_t              flags;
} rmx_module_desc_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Parameter Descriptor (For Automatable Parameters)
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_param_desc {
    uint32_t param_id;
    char     name[32];
    char     units[16];          /**< e.g. "dB", "Hz", "%" */
    float    min_value;
    float    max_value;
    float    default_value;
    float    step_size;
} rmx_param_desc_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Module Lifecycle Interface (Pure C Function Pointer Table)
 * ------------------------------------------------------------------------- */
typedef struct rmx_module_interface {
    /** Initialize module instance with optional JSON configuration */
    rmx_status_t (RMX_CALL *init)(rmx_handle_t* instance, const char* config_json);

    /** Prepare DSP / AI inference memory for given sample rate and block size */
    rmx_status_t (RMX_CALL *prepare_to_play)(rmx_handle_t instance, uint32_t sample_rate, uint32_t max_block_size);

    /** Real-time audio processing callback (Must be lock-free and allocation-free) */
    rmx_status_t (RMX_CALL *process_audio)(rmx_handle_t instance, const rmx_audio_buffer_t* in_buf, rmx_audio_buffer_t* out_buf);

    /** Real-time MIDI processing callback */
    rmx_status_t (RMX_CALL *process_midi)(rmx_handle_t instance, const rmx_midi_buffer_t* in_midi, rmx_midi_buffer_t* out_midi);

    /** Parameter management */
    rmx_status_t (RMX_CALL *set_parameter)(rmx_handle_t instance, uint32_t param_id, float value);
    rmx_status_t (RMX_CALL *get_parameter)(rmx_handle_t instance, uint32_t param_id, float* out_value);

    /** Optional custom UI interface (Returns NULL if module has no custom UI) */
    const rmx_ui_interface_t* (RMX_CALL *get_ui_interface)(rmx_handle_t instance);

    /** Reset internal state/buffers (e.g. on transport rewind/stop) */
    rmx_status_t (RMX_CALL *reset)(rmx_handle_t instance);

    /** Destroy module instance and free all internal resources */
    rmx_status_t (RMX_CALL *destroy)(rmx_handle_t instance);
} rmx_module_interface_t;

/* -------------------------------------------------------------------------
 * Exported Module Entry Points
 * -------------------------------------------------------------------------
 * Dynamic libraries (.dll / .dylib / .so) must export these symbols:
 */
typedef rmx_status_t (RMX_CALL *rmx_get_module_desc_fn)(rmx_module_desc_t* out_desc);
typedef rmx_status_t (RMX_CALL *rmx_create_module_fn)(const rmx_module_interface_t** out_interface, rmx_handle_t* out_instance);

#define RMX_ENTRY_GET_DESC   "rmx_get_module_desc"
#define RMX_ENTRY_CREATE_MOD "rmx_create_module"

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_MODULE_H */
