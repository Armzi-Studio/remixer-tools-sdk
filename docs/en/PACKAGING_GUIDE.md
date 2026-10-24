# Universal Multi-Target Packaging Guide (`.rmxpkg`)

This guide explains how to package dynamic module binaries (.dll, .dylib, .so) into a **Universal Multi-Target `.rmxpkg`** container using the `rmxpkg` command-line tool.

---

## 📦 What is `.rmxpkg`?

An `.rmxpkg` file is a cryptographically secured module container for Remixer Tools:
1. **Multi-Platform Support**: Houses binaries for Windows (`windows_x64`), macOS (`macos_arm64`, `macos_x64`), and Linux (`linux_x64`) inside a single distributor file.
2. **Cryptographic Checksum**: SHA-256 digest validates payload integrity against data corruption and tampering.
3. **Digital Signature**: Ed25519 digital signature verifies module provenance.
4. **Metadata & Declarations**: Embeds `manifest.json` describing module permissions, vendor information, and minimum SDK version.

---

## 🛠️ The `rmxpkg` Tool Syntax

The `rmxpkg` CLI supports both single-platform and multi-platform packaging:

### 1. Universal Multi-Target Pack (Recommended)

Bundle Windows, macOS, and Linux binaries into one `.rmxpkg`:

```bash
rmxpkg pack --manifest manifest.json \
  --target windows_x64=build/windows/my_module.dll \
  --target macos_arm64=build/macos/my_module.dylib \
  --target linux_x64=build/linux/my_module.so \
  --out dist/my_module.rmxpkg
```

### 2. Single-Platform Pack

Package a single binary (automatically tagged with host platform):

```bash
rmxpkg pack --manifest manifest.json --bin build/my_module.dll --out dist/my_module.rmxpkg
```

### 3. Verify an Existing Package

Inspect metadata and validate SHA-256 checksums:

```bash
rmxpkg verify dist/my_module.rmxpkg
```

### 4. Unpack a Package

Extract all platform binaries:
```bash
rmxpkg unpack dist/my_module.rmxpkg --out output_folder/
```

Extract only a specific target platform:
```bash
rmxpkg unpack dist/my_module.rmxpkg --platform macos_arm64 --out output_folder/my_module.dylib
```

---

## 📄 Manifest Specification (`manifest.json`)

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

### Standard Module Categories (`category`):
- `CORE_DSP`: General audio signal processing, EQ, dynamics
- `STEM_SEPARATOR`: AI/Deep learning stem isolation models (e.g. Demucs)
- `AUDIO_TO_MIDI`: Real-time pitch tracking and note conversion
- `DOWNLOADER`: Media ingestion and audio extraction engines
- `EFFECT`: Creative sound effects (reverb, delay, modulation)
- `SYNTHESIZER`: Virtual instruments and MIDI sound generators
- `ANALYZER`: Spectrum, LUFS metering, and visual analysis tools

---

## 🚀 Deployment Checklist

- [ ] Compile release binaries with optimization flags (`/O2` on MSVC, `-O3` on Clang/GCC).
- [ ] Ensure binaries have no missing external dynamic dependencies.
- [ ] Run `rmxpkg verify <module.rmxpkg>` to verify checksum integrity.
- [ ] Deploy to host by placing in `<app_directory>/modules/`.
