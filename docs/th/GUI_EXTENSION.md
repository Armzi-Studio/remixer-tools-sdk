# การสร้างหน้าต่างแสดงผลกราฟิกของโมดูล (`rmx_gui.h`)

คู่มือนี้อธิบายวิธีการที่นักพัฒนาภายนอก (Third-Party Developers) สามารถสร้างและฝังหน้าต่างส่วนติดต่อผู้ใช้ (User Interface) ของโมดูลลงในหน้าต่างหลักของ Remixer Tools ผ่านมาตรฐาน C-ABI GUI Extension (`rmx_ui_interface_t`)

---

## 🎨 ภาพรวมการทำงาน

Remixer Tools เปิดโอกาสให้โมดูลสามารถวาดหน้าต่าง UI ของตนเองลงในพื้นที่ทำงานหลัก (Workspace Canvas) ได้โดยตรง โดยไม่ต้องเข้าถึง Source Code ของโปรแกรมหลัก:

- **ไม่ต้องคอมไพล์โปรแกรมหลักใหม่ (Zero Host Recompilation):** ทำงานแบบไดนามิกผ่านฟังก์ชันพอยน์เตอร์ในไฟล์ `.dll` / `.dylib` / `.so` ของโมดูลเอง
- **กลมกลืนกับธีมของระบบ (Theme-Connected):** ได้รับชุดสีธีมปัจจุบัน (Dark / Light / Obsidian), Accent Color (สีส้ม, ม่วง, เขียว) และแบบอักษร Noto Sans จากโปรแกรมหลักโดยอัตโนมัติ
- **รองรับทุกเทคโนโลยี UI:** นักพัฒนาสามารถเลือกใช้วิธีวาด Native ของแต่ละ OS (Win32 GDI, Direct2D, Cocoa/Metal, Cairo) หรือใช้เอนจินกราฟิกข้ามแพลตฟอร์มอย่าง Skia, NanoVG, ImGui, หรือ JUCE ก็ได้

---

## 🏗️ การประกาศความสามารถรองรับ Custom GUI

ในฟังก์ชัน `rmx_get_module_desc` ของโมดูล ให้เพิ่ม Flag:

```c
RMX_API rmx_status_t RMX_CALL rmx_get_module_desc(rmx_module_desc_t* out_desc) {
    // ...
    out_desc->flags |= RMX_MOD_FLAG_HAS_CUSTOM_UI; // <-- แจ้งโปรแกรมหลักว่าโมดูลนี้มีหน้าต่าง UI
    return RMX_OK;
}
```

---

## 🖥️ การเขียนฟังก์ชันใน `rmx_ui_interface_t`

ตารางฟังก์ชันส่วนติดต่อผู้ใช้ประกอบด้วย 4 ฟังก์ชันพอยน์เตอร์:

```c
typedef struct rmx_ui_interface {
    rmx_status_t (RMX_CALL *render)(rmx_handle_t instance, void* draw_context, const rmx_rect_t* rect, const rmx_theme_context_t* theme);
    rmx_status_t (RMX_CALL *on_mouse_event)(rmx_handle_t instance, const rmx_mouse_event_t* event);
    rmx_status_t (RMX_CALL *attach_to_parent)(rmx_handle_t instance, void* parent_window_handle);
    rmx_status_t (RMX_CALL *detach)(rmx_handle_t instance);
} rmx_ui_interface_t;
```

### ตัวอย่างโค้ดที่สมบูรณ์:

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

// 1. ฟังก์ชันวาดภาพลงบน Canvas
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

    // 1. วาดการ์ดพื้นหลังขอบมนด้วยสีตามธีม
    RECT card_rc = { rect->left + 20, rect->top + 20, rect->right - 20, rect->bottom - 20 };
    HBRUSH bg_brush = CreateSolidBrush((COLORREF)theme->card_bg);
    HPEN border_pen = CreatePen(PS_SOLID, 1, (COLORREF)theme->card_border);
    SelectObject(hdc, bg_brush);
    SelectObject(hdc, border_pen);
    RoundRect(hdc, card_rc.left, card_rc.top, card_rc.right, card_rc.bottom, 12, 12);
    DeleteObject(bg_brush);
    DeleteObject(border_pen);

    // 2. เขียนหัวข้อด้วยฟอนต์ Noto Sans Title ของโปรแกรมหลัก
    SelectObject(hdc, (HFONT)theme->font_title);
    SetTextColor(hdc, (COLORREF)theme->text_primary);
    RECT title_rc = { card_rc.left + 24, card_rc.top + 24, card_rc.right - 24, card_rc.top + 54 };
    const wchar_t* title = theme->is_thai ? L"ตัวควบคุม Gain และระดับเสียง" : L"Custom Gain & Metering View";
    DrawTextW(hdc, title, -1, &title_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // 3. แสดงสถานะที่โต้ตอบได้
    SelectObject(hdc, (HFONT)theme->font_ui);
    SetTextColor(hdc, (COLORREF)theme->accent);
    wchar_t buf[128];
    wsprintfW(buf, L"จำนวนการคลิก: %d ครั้ง | ระดับ Gain ปัจจุบัน: %d%%", ctx->click_count, (int)(ctx->gain * 100));
    RECT status_rc = { card_rc.left + 24, card_rc.top + 60, card_rc.right - 24, card_rc.top + 90 };
    DrawTextW(hdc, buf, -1, &status_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
#endif

    return RMX_OK;
}

// 2. ฟังก์ชันรับ Event เมาส์
static rmx_status_t RMX_CALL my_ui_on_mouse_event(
    rmx_handle_t instance,
    const rmx_mouse_event_t* event
) {
    if (!instance || !event) return RMX_ERR_NULL_POINTER;
    MyModuleContext* ctx = (MyModuleContext*)instance;

    // ตรวจจับการคลิกเมาส์ซ้าย
    if (event->type == RMX_MOUSE_DOWN && (event->button_flags & 1)) {
        ctx->click_count++;
        return RMX_OK; // ส่ง RMX_OK เพื่อแจ้งว่า Event ถูกจัดการแล้ว
    }

    return RMX_ERR_GENERIC; // ส่งต่อให้ระบบถ้าไม่ได้จัดการ
}

// 3. ทำลายหรือ Detach เมื่อสลับแท็บ
static rmx_status_t RMX_CALL my_ui_detach(rmx_handle_t instance) {
    (void)instance;
    return RMX_OK;
}

// ตารางฟังก์ชัน GUI
static const rmx_ui_interface_t g_ui_interface = {
    my_ui_render,
    my_ui_on_mouse_event,
    NULL, // attach_to_parent (ไม่บังคับ ใช้กรณีต้องการแปะ Child HWND)
    my_ui_detach
};

// ส่งตารางฟังก์ชัน GUI ให้โปรแกรมหลัก
const rmx_ui_interface_t* RMX_CALL my_get_ui_interface(rmx_handle_t instance) {
    (void)instance;
    return &g_ui_interface;
}
```

---

## 📐 รายละเอียดโครงสร้าง Theme Context (`rmx_theme_context_t`)

| ฟิลด์ข้อมูล | ชนิดข้อมูลบน Windows | macOS / Linux | คำอธิบาย |
| :--- | :--- | :--- | :--- |
| `canvas_bg` | `COLORREF` (0x00BBGGRR) | `uint32_t` ARGB | สีพื้นหลัง Canvas ใหญ่ของโปรแกรม |
| `card_bg` | `COLORREF` | `uint32_t` ARGB | สีพื้นหลังของการ์ดหรือแผงควบคุม |
| `card_border` | `COLORREF` | `uint32_t` ARGB | สีเส้นขอบ 1px สำหรับแบ่งบล็อกการ์ด |
| `sidebar_bg` | `COLORREF` | `uint32_t` ARGB | สีพื้นหลังแผงควบคุมด้านข้าง |
| `accent` | `COLORREF` | `uint32_t` ARGB | สีไฮไลต์/สีเน้น (ส้ม Amber, ม่วง Neon, เขียว Emerald ฯลฯ) |
| `text_primary` | `COLORREF` | `uint32_t` ARGB | สีข้อความหลัก คมชัดสูง (ขาวนวล) |
| `text_secondary` | `COLORREF` | `uint32_t` ARGB | สีข้อความรองหรือคำอธิบายย่อย (เทาอ่อน) |
| `font_title` | `HFONT` | `void*` Native Font | แบบอักษร Noto Sans หัวข้อใหญ่ (18-20px) |
| `font_ui` | `HFONT` | `void*` Native Font | แบบอักษร Noto Sans สำหรับปุ่มและเมนู (13-14px) |
| `font_bold` | `HFONT` | `void*` Native Font | แบบอักษร Noto Sans ตัวหนา (13-14px) |
| `font_small` | `HFONT` | `void*` Native Font | แบบอักษร Noto Sans สำหรับป้ายกำกับขนาดเล็ก (11-12px) |
| `font_icon` | `HFONT` | `void*` Native Font | แบบอักษรไอคอนสัญลักษณ์ (Fluent / Segoe MDL2) |
| `is_thai` | `uint32_t` | `uint32_t` | มีค่าเป็น `1` เมื่อผู้ใช้เลือกภาษาไทย และ `0` สำหรับภาษาอังกฤษ |
