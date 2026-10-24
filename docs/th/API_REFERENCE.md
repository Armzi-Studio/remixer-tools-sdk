# เอกสารอ้างอิง Remixer Tools SDK C-ABI API Reference

เอกสารสรุปรายละเอียดฟังก์ชัน, โครงสร้างข้อมูล (Struct), ค่าสถานะ และตาราง Function Pointer ของ `remixer-tools-sdk`

---

## 📚 สารบัญไฟล์ Header

| ไฟล์ Header | หน้าที่และรายละเอียด |
| :--- | :--- |
| [`remixer.h`](#remixerh) | Master Header หลักที่รวมทุก Header ของ SDK เข้าด้วยกัน |
| [`rmx_types.h`](#rmx_typesh) | ชนิดข้อมูลพื้นฐาน, รหัสสถานะ Error (`rmx_status_t`), และมาโคร Export สำหรับแต่ละ OS |
| [`rmx_audio.h`](#rmx_audioh) | โครงสร้างบัฟเฟอร์เสียง Planar Float32 และการจัดช่องสัญญาณ Stems |
| [`rmx_midi.h`](#rmx_midih) | โครงสร้างบัฟเฟอร์ข้อความ MIDI และการตรวจจับระดับเสียงโน้ตดนตรี |
| [`rmx_module.h`](#rmx_moduleh) | วงจรชีวิตโมดูล (`rmx_module_interface_t`), ข้อมูลพารามิเตอร์ และ Entry Points ของ DLL |
| [`rmx_gui.h`](#rmx_guih) | ตาราง Interface สำหรับวาดหน้าต่าง UI, Event เมาส์ และชุดสีธีมของระบบ |
| [`rmx_ipc.h`](#rmx_ipch) | สตรีมมิ่งหน่วยความจำแชร์ข้าม Process (Shared Memory) และ Lock-Free Ring Buffer |
| [`rmx_license.h`](#rmx_licenseh) | การตรวจสอบ HWID ลายนิ้วมือเครื่อง, โทเค็นสิทธิ์ และ Heartbeat ป้องกันการแฮก |

---

## `rmx_types.h`

### รหัสสถานะการทำงาน (`rmx_status_t`)
```c
typedef enum rmx_status {
    RMX_OK                       = 0,   // สำเร็จสมบูรณ์
    RMX_ERR_GENERIC              = -1,  // ข้อผิดพลาดทั่วไป
    RMX_ERR_INVALID_ARGUMENT     = -2,  // อาร์กิวเมนต์ไม่ถูกต้อง
    RMX_ERR_OUT_OF_MEMORY        = -3,  // หน่วยความจำไม่เพียงพอ
    RMX_ERR_NOT_INITIALIZED      = -4,  // โมดูลยังไม่ได้ Initialize
    RMX_ERR_NULL_POINTER         = -6,  // มีพอยน์เตอร์เป็น NULL
    RMX_ERR_UNSUPPORTED          = -7,  // การทำงานไม่รองรับ
    RMX_ERR_BUFFER_OVERFLOW      = -50, // บัฟเฟอร์เสียงล้น
    RMX_ERR_BUFFER_UNDERFLOW     = -51, // บัฟเฟอร์เสียงขาดช่วง
    RMX_ERR_SAMPLE_RATE_MISMATCH = -52, // Sample Rate ไม่ตรงกัน
    RMX_ERR_CHANNEL_MISMATCH     = -53, // จำนวน Channel เสียงไม่ตรงกัน
    RMX_ERR_LICENSE_INVALID      = -100,// สิทธิ์การใช้งานไม่ถูกต้อง
    RMX_ERR_LICENSE_EXPIRED      = -101,// สิทธิ์การใช้งานหมดอายุ
    RMX_ERR_HWID_MISMATCH        = -103,// รหัสเครื่องไม่ตรงกับใบอนุญาต
    RMX_ERR_TAMPER_DETECTED      = -105 // ตรวจพบการพยายามดัดแปลงโมดูล
} rmx_status_t;
```

---

## `rmx_audio.h`

### โครงสร้างบัฟเฟอร์สัญญาณเสียง (`rmx_audio_buffer_t`)
```c
typedef struct rmx_audio_buffer {
    float**  channels;       /**< อาร์เรย์สัญญาณเสียงแบบ Planar [channel][frame] */
    uint32_t num_channels;   /**< จำนวน Channel เสียง (1=โมโน, 2=สเตอริโอ, 4=แยก 4 Stems) */
    uint32_t num_frames;     /**< จำนวนตัวอย่างเสียง (Sample Frames) ในรอบการประมวลผล */
    uint32_t sample_rate;    /**< อัตราความถี่เสียงในหน่วย Hz (เช่น 44100, 48000, 96000) */
} rmx_audio_buffer_t;
```

---

## `rmx_module.h`

### ตารางวงจรชีวิตโมดูล (`rmx_module_interface_t`)
```c
typedef struct rmx_module_interface {
    // กำหนดค่าเริ่มต้นให้กับอินสแตนซ์ของโมดูล
    rmx_status_t (RMX_CALL *init)(rmx_handle_t* instance, const char* config_json);

    // เตรียมความพร้อมของหน่วยความจำตาม Sample Rate และ Block Size
    rmx_status_t (RMX_CALL *prepare_to_play)(rmx_handle_t instance, uint32_t sample_rate, uint32_t max_block_size);

    // ประมวลผลเสียงแบบ Real-time (ห้ามมีการจอง Memory หรือใช้ Lock)
    rmx_status_t (RMX_CALL *process_audio)(rmx_handle_t instance, const rmx_audio_buffer_t* in_buf, rmx_audio_buffer_t* out_buf);

    // ประมวลผลข้อความ MIDI แบบ Real-time
    rmx_status_t (RMX_CALL *process_midi)(rmx_handle_t instance, const rmx_midi_buffer_t* in_midi, rmx_midi_buffer_t* out_midi);

    // ตั้งค่าและอ่านค่าพารามิเตอร์ของโมดูล
    rmx_status_t (RMX_CALL *set_parameter)(rmx_handle_t instance, uint32_t param_id, float value);
    rmx_status_t (RMX_CALL *get_parameter)(rmx_handle_t instance, uint32_t param_id, float* out_value);

    // ขอรับตารางส่วนติดต่อผู้ใช้ (Custom GUI Interface)
    const struct rmx_ui_interface* (RMX_CALL *get_ui_interface)(rmx_handle_t instance);

    // รีเซ็ตสถานะภายใน (เช่น เมื่อกดหยุดหรือย้อนกลับเพลง)
    rmx_status_t (RMX_CALL *reset)(rmx_handle_t instance);

    // ทำลายอินสแตนซ์และคืนหน่วยความจำทั้งหมด
    rmx_status_t (RMX_CALL *destroy)(rmx_handle_t instance);
} rmx_module_interface_t;
```

---

## `rmx_gui.h`

### ตารางส่วนติดต่อผู้ใช้ (`rmx_ui_interface_t`)
```c
typedef struct rmx_ui_interface {
    // วาดภาพหน้าต่างโมดูลลงบน Drawing Context ของโปรแกรมหลัก
    rmx_status_t (RMX_CALL *render)(rmx_handle_t instance, void* draw_context, const rmx_rect_t* rect, const rmx_theme_context_t* theme);

    // จัดการ Event เมาส์เมื่อผู้ใช้คลิกหรือเลื่อนเมาส์ในพื้นที่โมดูล
    rmx_status_t (RMX_CALL *on_mouse_event)(rmx_handle_t instance, const rmx_mouse_event_t* event);

    // แนบหน้าต่างย่อย (Child Window Handle เช่น HWND บน Windows)
    rmx_status_t (RMX_CALL *attach_to_parent)(rmx_handle_t instance, void* parent_window_handle);

    // ทำลายการแสดงผลหรือ Detach เมื่อสลับแท็บ
    rmx_status_t (RMX_CALL *detach)(rmx_handle_t instance);
} rmx_ui_interface_t;
```
