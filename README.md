# Remixer Tools SDK (`remixer-sdk`)

[![License: Proprietary / Dual](https://img.shields.io/badge/License-Armzi%20Studio-blue.svg)](#)
[![Standard: C11 / C++17](https://img.shields.io/badge/Standard-C11%20%2F%20C%2B%2B17-green.svg)](#)
[![CMake: 3.20+](https://img.shields.io/badge/CMake-3.20%2B-orange.svg)](#)

ชุดมาตรฐาน C-ABI Headers, Data Structures, และ Audio/MIDI/IPC Streaming Protocols สำหรับระบบ **Remixer Tools** ออกแบบมาเพื่อให้นักพัฒนาทั้งภายในและภายนอก (Third-Party Developers) สามารถสร้างส่วนขยาย (Plugins/Modules) หรือเชื่อมต่อกับ Audio Core Engine และ Standalone Application ได้อย่างไร้รอยต่อ

---

## 📁 โครงสร้าง SDK (Directory Layout)

```
remixer-tools-sdk/
├── include/
│   └── remixer/
│       ├── remixer.h       # Master include header
│       ├── rmx_types.h     # Primitives, Versioning, Error codes (rmx_status_t)
│       ├── rmx_audio.h     # Planar Float32 buffer formats, Stem layouts
│       ├── rmx_midi.h      # Real-time MIDI events, Pitch detection data
│       ├── rmx_ipc.h        # Shared memory headers, SPSC lock-free atomic rings
│       ├── rmx_module.h    # Dynamic module vtable & lifecycle hooks
│       └── rmx_license.h   # HWID fingerprinting, Anti-tamper heartbeat
├── examples/
│   └── dummy_gain_module/  # ตัวอย่างการเขียน Audio DSP C Module
├── CMakeLists.txt
└── README.md
```

---

## 🚀 การนำไปใช้งานในโปรเจกต์ (Integration)

### 1. ด้วย CMake (FetchContent หรือ Subdirectory)

```cmake
# เพิ่ม remixer-sdk เข้าสู่โปรเจกต์ของคุณ
add_subdirectory(path/to/remixer-tools-sdk)

# เชื่อมต่อเข้ากับ Target ของคุณ
target_link_libraries(my_audio_module PRIVATE remixer::sdk)
```

### 2. ตัวอย่างการสร้างโมดูล C-ABI (`rmx_module_interface_t`)

```c
#include <remixer/remixer.h>

static rmx_status_t RMX_CALL my_process_audio(rmx_handle_t instance,
                                             const rmx_audio_buffer_t* in_buf,
                                             rmx_audio_buffer_t* out_buf) {
    // Zero-allocation, Lock-free Real-time audio processing
    for (uint32_t ch = 0; ch < in_buf->num_channels; ++ch) {
        for (uint32_t i = 0; i < in_buf->num_frames; ++i) {
            out_buf->channels[ch][i] = in_buf->channels[ch][i] * 0.5f; // ตัวอย่างลด Gain 6dB
        }
    }
    return RMX_OK;
}
```

---

## 🛡️ กฎความเสถียรของ C-ABI (ABI Stability Guarantees)

1. **Pure C11 Linkage**: ทุก Interface สื่อสารผ่าน `extern "C"` เพื่อข้ามข้อจำกัดของ C++ Name Mangling
2. **Explicit Struct Alignment**: ใช้ `#pragma pack(push, 8)` เพื่อรับประกัน Data Alignment เดียวกันทั้งบน MSVC, Clang, และ GCC
3. **Opaque Pointers**: ซ่อน Internal State ของโมดูลไว้หลัง `rmx_handle_t` (`void*`)
4. **Zero-Allocation in Process Block**: `process_audio` และ `process_midi` ต้องไม่มีการจัดสรร Memory ใน Real-Time Audio Thread

---

## 📄 ลิขสิทธิ์ (License)

Copyright (c) 2026 Armzi Studio. All rights reserved.