/**
 * @file rmx_gui.h
 * @brief Dynamic Module GUI Extension Interface (C-ABI standard).
 * Allows any 3rd-party module to embed, draw, and handle user interactions
 * in the Remixer host without modifying the host application code.
 * @copyright Copyright (c) 2026 Armzi Studio. All rights reserved.
 */

#ifndef REMIXER_GUI_H
#define REMIXER_GUI_H

#include "rmx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Basic GUI Geometry & Theme Descriptors
 * ------------------------------------------------------------------------- */
#pragma pack(push, 8)
typedef struct rmx_rect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} rmx_rect_t;

typedef struct rmx_theme_context {
    uint32_t canvas_bg;      /**< 0x00BBGGRR (Win32 COLORREF format) */
    uint32_t card_bg;
    uint32_t card_border;
    uint32_t sidebar_bg;
    uint32_t accent;
    uint32_t text_primary;
    uint32_t text_secondary;
    void*    font_title;     /**< Platform native font handle (e.g. HFONT on Windows) */
    void*    font_ui;
    void*    font_bold;
    void*    font_small;
    void*    font_icon;
    uint32_t is_thai;        /**< 1 if current host UI language is Thai, 0 for English */
} rmx_theme_context_t;

/* -------------------------------------------------------------------------
 * Mouse & Input Interaction Events
 * ------------------------------------------------------------------------- */
typedef enum rmx_mouse_type {
    RMX_MOUSE_DOWN   = 1,
    RMX_MOUSE_UP     = 2,
    RMX_MOUSE_MOVE   = 3,
    RMX_MOUSE_WHEEL  = 4,
    RMX_MOUSE_DBLCLK = 5
} rmx_mouse_type_t;

typedef struct rmx_mouse_event {
    rmx_mouse_type_t type;
    int32_t          x;
    int32_t          y;
    uint32_t         button_flags; /**< Bit 0: Left, Bit 1: Right, Bit 2: Middle */
    int32_t          wheel_delta;
} rmx_mouse_event_t;
#pragma pack(pop)

/* -------------------------------------------------------------------------
 * Dynamic GUI Interface Table
 * Function pointer table implemented by modules declaring RMX_MOD_FLAG_HAS_CUSTOM_UI
 * ------------------------------------------------------------------------- */
typedef struct rmx_ui_interface {
    /**
     * @brief Render the module's custom UI onto the host's drawing context.
     * @param instance Module instance handle.
     * @param draw_context Platform-native drawing context (e.g. HDC on Windows).
     * @param rect Bounding rectangle allocated for this module view.
     * @param theme Current active theme palette and typography handles from host.
     */
    rmx_status_t (RMX_CALL *render)(rmx_handle_t instance, void* draw_context, const rmx_rect_t* rect, const rmx_theme_context_t* theme);

    /**
     * @brief Handle mouse interaction events.
     * @param instance Module instance handle.
     * @param event Mouse event data.
     * @return RMX_OK if handled/consumed, non-zero if ignored/unhandled.
     */
    rmx_status_t (RMX_CALL *on_mouse_event)(rmx_handle_t instance, const rmx_mouse_event_t* event);

    /**
     * @brief Optional: Attach native child window (e.g. HWND on Windows, NSView on Mac).
     * Set to NULL if module uses canvas rendering via `render`.
     */
    rmx_status_t (RMX_CALL *attach_to_parent)(rmx_handle_t instance, void* parent_window_handle);

    /**
     * @brief Called when view is hidden, detached, or tab switched away.
     */
    rmx_status_t (RMX_CALL *detach)(rmx_handle_t instance);
} rmx_ui_interface_t;

#ifdef __cplusplus
}
#endif

#endif /* REMIXER_GUI_H */
