# Remixer Tools SDK (`remixer-sdk`)

[![License: Proprietary](https://img.shields.io/badge/License-Armzi%20Studio-blue.svg)](#)
[![Standard: C11 / C++17](https://img.shields.io/badge/Standard-C11%20%2F%20C%2B%2B17-green.svg)](#)
[![Packaging: Universal .rmxpkg v2](https://img.shields.io/badge/Packaging-Universal%20.rmxpkg%20v2-purple.svg)](#)
[![ABI: Guaranteed C-ABI](https://img.shields.io/badge/ABI-Guaranteed%20C--ABI-orange.svg)](#)

[English](#english) | [ภาษาไทย](#ภาษาไทย)

---

<a name="english"></a>
## 🌐 English

The **Remixer Tools SDK** provides a high-performance, rock-solid pure C-ABI standard, shared data structures, real-time audio/MIDI streaming protocols, and UI extensions for the **Remixer Tools** modular workstation ecosystem. It allows first-party and third-party developers to build isolated, dynamically loaded modules (`.dll` / `.dylib` / `.so`) and distribute them as cross-platform `.rmxpkg` packages without recompiling the host application.

### 📚 Documentation Hub

| Guide | Description |
| :--- | :--- |
| 🚀 [**Getting Started Guide**](docs/en/GETTING_STARTED.md) | Architecture overview, module lifecycle, CMake setup, and building your first dynamic library. |
| 🎨 [**GUI Extension Guide**](docs/en/GUI_EXTENSION.md) | Integrating platform-independent custom UI with `rmx_ui_interface_t`, rendering loops, and mouse events. |
| 📦 [**Packaging Guide**](docs/en/PACKAGING_GUIDE.md) | Creating Universal Multi-Target `.rmxpkg` bundles containing binaries for Windows, macOS, and Linux. |
| 📖 [**C-ABI API Reference**](docs/en/API_REFERENCE.md) | Exhaustive reference for all SDK header files, structs, enums, error codes, and exported entry points. |

### 📁 Directory Layout

```
remixer-tools-sdk/
├── include/
│   └── remixer/
│       ├── remixer.h         # Master umbrella header
│       ├── rmx_types.h       # Primitives, error codes, and platform macros
│       ├── rmx_audio.h       # Planar Float32 audio buffers & channel layouts
│       ├── rmx_midi.h        # Real-time MIDI events & pitch detection
│       ├── rmx_module.h      # Module lifecycle vtable & DLL entry points
│       ├── rmx_gui.h         # Platform-agnostic GUI rendering & input table
│       ├── rmx_ipc.h         # Shared memory streaming & lock-free SPSC rings
│       └── rmx_license.h     # Machine fingerprinting & anti-tamper heartbeat
├── docs/
│   ├── en/                   # Full English documentation
│   └── th/                   # คู่มือภาษาไทยฉบับสมบูรณ์
├── examples/
│   └── dummy_gain_module/    # Reference Audio DSP C Module implementation
├── CMakeLists.txt
└── README.md
```

### ⚡ Quick Start: CMake Integration

```cmake
# Add SDK to your project
add_subdirectory(path/to/remixer-tools-sdk)

# Link your dynamic library target
add_library(my_module SHARED my_module.c)
target_link_libraries(my_module PRIVATE remixer::sdk)
```

---

<a name="ภาษาไทย"></a>
## 🇹🇭 ภาษาไทย

**Remixer Tools SDK** คือชุดส่วนหัว (Headers) มาตรฐาน C-ABI ประสิทธิภาพสูง พร้อมโครงสร้างข้อมูล โพรโทคอลสตรีมมิ่งเสียง/MIDI แบบเรียลไทม์ และระบบขยายส่วนติดต่อผู้ใช้ (GUI Extension) สำหรับระบบ **Remixer Tools** ออกแบบมาเพื่อให้นักพัฒนาทั้งภายในและภายนอกสามารถสร้างปลั๊กอิน/โมดูลในรูปแบบ Dynamic Library (`.dll`, `.dylib`, `.so`) และส่งมอบผ่านแพ็กเกจข้ามแพลตฟอร์ม `.rmxpkg` ได้อย่างอิสระโดยไม่จำเป็นต้องแก้ไขหรือคอมไพล์โปรแกรมหลักใหม่

### 📚 สารบัญคู่มือและเอกสารการใช้งาน

| เอกสารคู่มือ | รายละเอียด |
| :--- | :--- |
| 🚀 [**คู่มือเริ่มต้นพัฒนาโมดูล**](docs/th/GETTING_STARTED.md) | ภาพรวมสถาปัตยกรรม, วงจรชีวิตของโมดูล, การตั้งค่า CMake และการสร้างโมดูลแรก |
| 🎨 [**การสร้างหน้าต่างแสดงผลกราฟิก (GUI)**](docs/th/GUI_EXTENSION.md) | การต่อยอด UI ด้วย `rmx_ui_interface_t`, การวาดหน้าจอ (Rendering) และการรับ Event เมาส์ |
| 📦 [**คู่มือการแพ็กเกจ Universal Multi-Target**](docs/th/PACKAGING_GUIDE.md) | การรวมไบนารีของ Windows, macOS และ Linux ไว้ในไฟล์ `.rmxpkg` เดียวกันด้วย `rmxpkg` CLI |
| 📖 [**เอกสารอ้างอิง C-ABI API**](docs/th/API_REFERENCE.md) | สรุปโครงสร้างข้อมูล (Struct), ค่าสถานะ (Enum), Function Pointers และ Entry Points ทั้งหมด |

### 🛡️ กฎความเสถียรของ C-ABI (ABI Stability Guarantees)

1. **Pure C11 Linkage**: ทุก Interface สื่อสารผ่าน `extern "C"` ปราศจาก C++ Name Mangling
2. **Explicit Struct Alignment**: ใช้ `#pragma pack(push, 8)` กำหนด Alignment ตรงกันบน MSVC, Clang และ GCC
3. **Opaque Handles**: ซ่อน State ภายในไว้หลัง `rmx_handle_t` (`void*`)
4. **Zero-Allocation Audio Thread**: ฟังก์ชันประมวลผลเสียงห้ามเรียก Dynamic Memory Allocation หรือใช้ Lock
5. **Decoupled GUI Rendering**: โปรแกรมหลักและโมดูลส่งต่อเฉพาะ Drawing Context แบบไม่ผูกติดเฟรมเวิร์ก

---

## 📄 ลิขสิทธิ์ (License)

Copyright (c) 2026 Armzi Studio. All rights reserved.