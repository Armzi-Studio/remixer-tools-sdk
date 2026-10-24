# Remixer Tools SDK C-ABI API Reference

Comprehensive reference for all C-ABI headers, types, and function pointer tables in `remixer-tools-sdk`.

---

## 📚 Header Catalog

| Header File | Purpose |
| :--- | :--- |
| [`remixer.h`](#remixerh) | Master umbrella header including all SDK definitions |
| [`rmx_types.h`](#rmx_typesh) | Primitives, status codes, platform export macros, and opaque handles |
| [`rmx_audio.h`](#rmx_audioh) | Planar 32-bit float audio buffers and multi-channel stem configurations |
| [`rmx_midi.h`](#rmx_midih) | Real-time MIDI message buffers and pitch detection structures |
| [`rmx_module.h`](#rmx_moduleh) | Module lifecycle vtable, parameter descriptors, and DLL entry points |
| [`rmx_gui.h`](#rmx_guih) | Platform-independent GUI extension table, mouse events, and theme contexts |
| [`rmx_ipc.h`](#rmx_ipch) | Inter-process shared memory streaming and lock-free SPSC atomic ring buffers |
| [`rmx_license.h`](#rmx_licenseh) | HWID machine fingerprinting, license token verification, and anti-tamper heartbeat |

---

## `rmx_types.h`

### Return Status Codes (`rmx_status_t`)
```c
typedef enum rmx_status {
    RMX_OK                       = 0,
    RMX_ERR_GENERIC              = -1,
    RMX_ERR_INVALID_ARGUMENT     = -2,
    RMX_ERR_OUT_OF_MEMORY        = -3,
    RMX_ERR_NOT_INITIALIZED      = -4,
    RMX_ERR_NULL_POINTER         = -6,
    RMX_ERR_UNSUPPORTED          = -7,
    RMX_ERR_BUFFER_OVERFLOW      = -50,
    RMX_ERR_BUFFER_UNDERFLOW     = -51,
    RMX_ERR_SAMPLE_RATE_MISMATCH = -52,
    RMX_ERR_CHANNEL_MISMATCH     = -53,
    RMX_ERR_LICENSE_INVALID      = -100,
    RMX_ERR_LICENSE_EXPIRED      = -101,
    RMX_ERR_HWID_MISMATCH        = -103,
    RMX_ERR_TAMPER_DETECTED      = -105
} rmx_status_t;
```

---

## `rmx_audio.h`

### Audio Buffer Representation (`rmx_audio_buffer_t`)
```c
typedef struct rmx_audio_buffer {
    float**  channels;       /**< Planar float32 array [channel][frame] */
    uint32_t num_channels;   /**< Number of active audio channels (1=Mono, 2=Stereo, 4=Stems) */
    uint32_t num_frames;     /**< Number of sample frames in current processing block */
    uint32_t sample_rate;    /**< Sample rate in Hz (e.g. 44100, 48000, 96000) */
} rmx_audio_buffer_t;
```

---

## `rmx_module.h`

### Module Lifecycle Table (`rmx_module_interface_t`)
```c
typedef struct rmx_module_interface {
    rmx_status_t (RMX_CALL *init)(rmx_handle_t* instance, const char* config_json);
    rmx_status_t (RMX_CALL *prepare_to_play)(rmx_handle_t instance, uint32_t sample_rate, uint32_t max_block_size);
    rmx_status_t (RMX_CALL *process_audio)(rmx_handle_t instance, const rmx_audio_buffer_t* in_buf, rmx_audio_buffer_t* out_buf);
    rmx_status_t (RMX_CALL *process_midi)(rmx_handle_t instance, const rmx_midi_buffer_t* in_midi, rmx_midi_buffer_t* out_midi);
    rmx_status_t (RMX_CALL *set_parameter)(rmx_handle_t instance, uint32_t param_id, float value);
    rmx_status_t (RMX_CALL *get_parameter)(rmx_handle_t instance, uint32_t param_id, float* out_value);
    const struct rmx_ui_interface* (RMX_CALL *get_ui_interface)(rmx_handle_t instance);
    rmx_status_t (RMX_CALL *reset)(rmx_handle_t instance);
    rmx_status_t (RMX_CALL *destroy)(rmx_handle_t instance);
} rmx_module_interface_t;
```

---

## `rmx_gui.h`

### GUI Interface Table (`rmx_ui_interface_t`)
```c
typedef struct rmx_ui_interface {
    rmx_status_t (RMX_CALL *render)(rmx_handle_t instance, void* draw_context, const rmx_rect_t* rect, const rmx_theme_context_t* theme);
    rmx_status_t (RMX_CALL *on_mouse_event)(rmx_handle_t instance, const rmx_mouse_event_t* event);
    rmx_status_t (RMX_CALL *attach_to_parent)(rmx_handle_t instance, void* parent_window_handle);
    rmx_status_t (RMX_CALL *detach)(rmx_handle_t instance);
} rmx_ui_interface_t;
```
