# Custom Module GUI Extension (`rmx_gui.h`)

This guide explains how third-party developers can embed a custom User Interface into the Remixer Tools host using the C-ABI GUI extension (`rmx_ui_interface_t`).

---

## 🎨 Overview

Remixer Tools allows modules to draw custom UI directly onto the central application workspace without needing access to the host's source code or internal UI engine.

- **Zero Host Recompilation**: Works dynamically via function pointers in your module's `.dll` / `.dylib` / `.so`.
- **Theme-Connected**: Automatically receives the active dark/light palette, accent color, and native typography handles (`HFONT`, `CGFontRef`, `cairo_font_face_t`).
- **Flexible UI Backend**: You can draw using platform-native APIs (Win32 GDI, Direct2D, Cocoa/Metal, Cairo) or embed cross-platform vector/GUI engines (Skia, NanoVG, ImGui, JUCE).

---

## 🏗️ Declaring Custom GUI Support

In your module's `rmx_get_module_desc`:

```c
RMX_API rmx_status_t RMX_CALL rmx_get_module_desc(rmx_module_desc_t* out_desc) {
    // ...
    out_desc->flags |= RMX_MOD_FLAG_HAS_CUSTOM_UI; // <-- Declare GUI capability
    return RMX_OK;
}
```

---

## 🖥️ Implementing `rmx_ui_interface_t`

The GUI extension table consists of 4 function pointers:

```c
typedef struct rmx_ui_interface {
    rmx_status_t (RMX_CALL *render)(rmx_handle_t instance, void* draw_context, const rmx_rect_t* rect, const rmx_theme_context_t* theme);
    rmx_status_t (RMX_CALL *on_mouse_event)(rmx_handle_t instance, const rmx_mouse_event_t* event);
    rmx_status_t (RMX_CALL *attach_to_parent)(rmx_handle_t instance, void* parent_window_handle);
    rmx_status_t (RMX_CALL *detach)(rmx_handle_t instance);
} rmx_ui_interface_t;
```

### Complete Implementation Example:

```c
#include <remixer/remixer.h>
#include <remixer/rmx_gui.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

typedef struct {
    float gain;
    int click_count;
} MyModuleContext;

static rmx_status_t RMX_CALL my_ui_render(
    rmx_handle_t instance,
    void* draw_context,
    const rmx_rect_t* rect,
    const rmx_theme_context_t* theme
) {
    if (!instance || !draw_context || !rect || !theme) return RMX_ERR_NULL_POINTER;
    MyModuleContext* ctx = (MyModuleContext*)instance;

#if defined(_WIN32)
    HDC hdc = (HDC)draw_context;
    SetBkMode(hdc, TRANSPARENT);

    // 1. Draw rounded container card using host theme colors
    RECT card_rc = { rect->left + 20, rect->top + 20, rect->right - 20, rect->bottom - 20 };
    HBRUSH bg_brush = CreateSolidBrush((COLORREF)theme->card_bg);
    HPEN border_pen = CreatePen(PS_SOLID, 1, (COLORREF)theme->card_border);
    SelectObject(hdc, bg_brush);
    SelectObject(hdc, border_pen);
    RoundRect(hdc, card_rc.left, card_rc.top, card_rc.right, card_rc.bottom, 12, 12);
    DeleteObject(bg_brush);
    DeleteObject(border_pen);

    // 2. Draw module title with host font
    SelectObject(hdc, (HFONT)theme->font_title);
    SetTextColor(hdc, (COLORREF)theme->text_primary);
    RECT title_rc = { card_rc.left + 24, card_rc.top + 24, card_rc.right - 24, card_rc.top + 54 };
    DrawTextW(hdc, L"Custom Gain & Metering View", -1, &title_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // 3. Draw interactive status
    SelectObject(hdc, (HFONT)theme->font_ui);
    SetTextColor(hdc, (COLORREF)theme->accent);
    wchar_t buf[64];
    wsprintfW(buf, L"User Clicks: %d | Current Gain: %d%%", ctx->click_count, (int)(ctx->gain * 100));
    RECT status_rc = { card_rc.left + 24, card_rc.top + 60, card_rc.right - 24, card_rc.top + 90 };
    DrawTextW(hdc, buf, -1, &status_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
#endif

    return RMX_OK;
}

static rmx_status_t RMX_CALL my_ui_on_mouse_event(
    rmx_handle_t instance,
    const rmx_mouse_event_t* event
) {
    if (!instance || !event) return RMX_ERR_NULL_POINTER;
    MyModuleContext* ctx = (MyModuleContext*)instance;

    if (event->type == RMX_MOUSE_DOWN && (event->button_flags & 1)) {
        ctx->click_count++;
        return RMX_OK; // Event consumed
    }

    return RMX_ERR_GENERIC; // Ignored / pass-through
}

static rmx_status_t RMX_CALL my_ui_detach(rmx_handle_t instance) {
    (void)instance;
    return RMX_OK;
}

static const rmx_ui_interface_t g_ui_interface = {
    my_ui_render,
    my_ui_on_mouse_event,
    NULL, // attach_to_parent (Optional child window handle)
    my_ui_detach
};

// Return GUI interface table from module lifecycle
const rmx_ui_interface_t* RMX_CALL my_get_ui_interface(rmx_handle_t instance) {
    (void)instance;
    return &g_ui_interface;
}
```

---

## 📐 Theme Context Reference (`rmx_theme_context_t`)

| Field | Windows Type | macOS / Linux | Description |
| :--- | :--- | :--- | :--- |
| `canvas_bg` | `COLORREF` (0x00BBGGRR) | `uint32_t` ARGB | Main background canvas color |
| `card_bg` | `COLORREF` | `uint32_t` ARGB | Surface / container card background |
| `card_border` | `COLORREF` | `uint32_t` ARGB | Subtle 1px border stroke color |
| `sidebar_bg` | `COLORREF` | `uint32_t` ARGB | Contrast panel background |
| `accent` | `COLORREF` | `uint32_t` ARGB | Active user accent color (Amber, Neon, Emerald, etc.) |
| `text_primary` | `COLORREF` | `uint32_t` ARGB | High-contrast text color |
| `text_secondary` | `COLORREF` | `uint32_t` ARGB | Muted / description text color |
| `font_title` | `HFONT` | `void*` Native Font | Semibold 18-20px Noto Sans font handle |
| `font_ui` | `HFONT` | `void*` Native Font | Regular 13-14px Noto Sans UI font handle |
| `font_bold` | `HFONT` | `void*` Native Font | Bold 13-14px Noto Sans font handle |
| `font_small` | `HFONT` | `void*` Native Font | 11-12px Noto Sans secondary label font handle |
| `font_icon` | `HFONT` | `void*` Native Font | Fluent / Segoe MDL2 icon font handle |
| `is_thai` | `uint32_t` | `uint32_t` | `1` if host language is Thai, `0` for English |
