# คู่มือการแพ็กเกจโมดูลแบบ Universal Multi-Target (`.rmxpkg`)

คู่มือนี้อธิบายวิธีการแพ็กเกจไฟล์ไบนารีของโมดูล (.dll, .dylib, .so) เข้าสู่ไฟล์คอนเทนเนอร์ความปลอดภัย **Universal Multi-Target `.rmxpkg`** ด้วยเครื่องมือคอมมานด์ไลน์ `rmxpkg`

---

## 📦 ไฟล์ `.rmxpkg` คืออะไร?

ไฟล์ `.rmxpkg` คือมาตรฐานไฟล์แพ็กเกจปลั๊กอินสำหรับระบบ Remixer Tools:
1. **รองรับหลายระบบปฏิบัติการพร้อมกัน (Universal Multi-Target):** บรรจุไฟล์ไบนารีของ Windows (`windows_x64`), macOS (`macos_arm64`, `macos_x64`), และ Linux (`linux_x64`) รวมอยู่ในไฟล์เดียว
2. **ความปลอดภัยและการตรวจสอบความถูกต้อง:** มี Checksum SHA-256 ป้องกันไฟล์เสียหายหรือถูกดัดแปลง (Anti-Tampering)
3. **การรับรองตัวตน (Digital Signature):** ลงลายมือชื่อดิจิทัล Ed25519 ระบุผู้พัฒนา
4. **ข้อมูลโมดูล (Metadata):** ฝังไฟล์ `manifest.json` ระบุเวอร์ชัน, หมวดหมู่, และสิทธิ์การทำงาน

---

## 🛠️ ไวยากรณ์คำสั่งของเครื่องมือ `rmxpkg`

เครื่องมือ `rmxpkg` รองรับทั้งการแพ็กแยก OS และการแพ็กแบบ Universal รวมทุก OS:

### 1. การแพ็กแบบ Universal รวมทุก OS (แนะนำสูงสุด)

นำไฟล์ของ Windows, macOS, และ Linux มารวมในไฟล์ `.rmxpkg` เดียว:

```bash
rmxpkg pack --manifest manifest.json \
  --target windows_x64=build/windows/my_module.dll \
  --target macos_arm64=build/macos/my_module.dylib \
  --target linux_x64=build/linux/my_module.so \
  --out dist/my_module.rmxpkg
```

### 2. การแพ็กแบบ Single-Platform (สำหรับ OS ปัจจุบัน)

แพ็กเฉพาะไบนารีตัวเดียว (ระบบจะตรวจจับชื่อ OS ของเครื่องปัจจุบันให้อัตโนมัติ):

```bash
rmxpkg pack --manifest manifest.json --bin build/my_module.dll --out dist/my_module.rmxpkg
```

### 3. การตรวจสอบความสมบูรณ์ของแพ็กเกจ (Verify)

ตรวจสอบความถูกต้องของลายเซ็นและ Checksum SHA-256:

```bash
rmxpkg verify dist/my_module.rmxpkg
```

### 4. การแตกไฟล์แพ็กเกจ (Unpack)

แตกไฟล์ทุก Platform ออกมาเป็นโฟลเดอร์:
```bash
rmxpkg unpack dist/my_module.rmxpkg --out output_folder/
```

แตกเฉพาะ Platform ที่ต้องการ:
```bash
rmxpkg unpack dist/my_module.rmxpkg --platform macos_arm64 --out output_folder/my_module.dylib
```

---

## 📄 โครงสร้างไฟล์ Manifest (`manifest.json`)

```json
{
    "id": "com.example.my_module",
    "name": "My Custom Module",
    "version": "1.0.0",
    "category": "CORE_DSP",
    "vendor": "Acme Audio",
    "description": "High-performance dynamic audio processor",
    "package_type": "universal",
    "supported_platforms": [
        "windows_x64",
        "macos_arm64",
        "linux_x64"
    ],
    "api_version": "1.0.0"
}
```

### หมวดหมู่โมดูลมาตรฐาน (`category`):
- `CORE_DSP`: ประมวลผลสัญญาณเสียงทั่วไป, ฟิลเตอร์, EQ, Compressor
- `STEM_SEPARATOR`: โมเดล AI แยกเสียงเครื่องดนตรี / Stems (เช่น Demucs)
- `AUDIO_TO_MIDI`: ตรวจจับระดับเสียงแปลงเป็นโน้ต MIDI แบบ Real-time
- `DOWNLOADER`: เครื่องยนต์ดาวน์โหลดและแปลงไฟล์เสียงออนไลน์
- `EFFECT`: ซาวด์เอฟเฟกต์สร้างสรรค์ (Reverb, Delay, Modulation)
- `SYNTHESIZER`: เครื่องดนตรีเสมือน (Virtual Instrument / Synth)
- `ANALYZER`: เครื่องมือวัดระดับเสียง Spectrum, VU Meter, LUFS Analyzer

---

## 🚀 ขั้นตอนการนำไปติดตั้งใช้งานจริง

1. คอมไพล์โมดูลเป็น Release โหมดเพื่อประสิทธิภาพสูงสุด
2. สั่งแพ็กเป็นไฟล์ `.rmxpkg` ผ่านคำสั่ง `rmxpkg pack`
3. นำไฟล์ `.rmxpkg` ไปวางในโฟลเดอร์ `modules/` ของโปรแกรม Remixer Tools โปรแกรมจะตรวจจับและเพิ่มโมดูลเข้าสู่ Sidebar อัตโนมัติครับ
