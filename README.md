# OGRES File Format & Library

<img width="640" height="64" alt="ogreslogo" src="https://github.com/user-attachments/assets/20791b94-a8d1-4b58-aedb-0bcaaa1b2114" />

**OGRES (Open Game RESources)** is a compact binary resource format designed to bundle game resources into a single structured file. This repository provides a C library for loading and extracting image layers from `.ogres` files, along with a simple Win32 viewer for testing and visualization.

---

## 🛠 Features

### Core Format
- ✅ **Multi-layer container** - Pack multiple images into one file
- ⚡ **Zero-copy loading** - Direct memory mapping support
- 📦 **Compact binary format** - Minimal overhead (11-byte header)
- 🎮 **Game-optimized** - BGR pixel format for DirectX compatibility

### Library
- 🚀 **Single-header C library** (`ogres.h`)
- 🧩 **Zero dependencies** - Just standard C and Win32
- 💾 **Memory-safe** - Built-in resource cleanup
- 🔄 **Thread-safe** operations

### Tools
- 👀 **Win32 Viewer** - Visualize layers with navigation
- 🐍 **Python Packer GUI** - Drag-and-drop asset bundling


---

## 📁 File Structure

```
OGRES/
├── ogres.h            # Public header for loading OGRES data
├── ogres.c            # OGRES loader implementation
├── OGRES Tests/       # Win32 application to display OGRES images
│   └── main.c
```

---

## 🛠 Build Instructions

### 🔗 Visual Studio (Recommended)

```bash
git clone https://github.com/your-repo/OGRES.git
cd OGRES
msbuild OGRES.sln /p:Configuration=Release
```

---

## 📜 File Format Specification

### Header (11 bytes)
| Offset | Type  | Field        | Description                  |
|--------|-------|--------------|------------------------------|
| 0x00   | char[5] | Magic       | `"OGRES"` identifier         |
| 0x05   | uint16 | Layer Count  | Number of image layers       |
| 0x07   | uint32 | Total Size   | Combined size of all layers  |


Each layer contains:

| Field      | Type    | Description                   |
| ---------- | ------- | ----------------------------- |
| Width      | WORD    | Width of the image in pixels  |
| Height     | WORD    | Height of the image in pixels |
| Layer Size | WORD    | Size of the layer data        |
| Pixels     | BYTE\[] | Packed BGR triples (no alpha) |

---

## 💻 Example Usage (C)

```c
ogres_t ogres;
if (OGRES_Load(&ogres, "textures.ogres")) {
    image_t img;
    if (OGRES_GetImage(&ogres, &img, 0)) {
        // use img.pixels...
        OGRES_FreeImage(&img);
    }
    OGRES_Free(&ogres);
}
```
---

## 📜 API Reference
| Function | Parameters | Description |
| -------- | ---------- | ----------- |
| `OGRES_Load` | `ogres_t*, const char*` | Loads OGRES file |
| `OGRES_GetImage` | `ogres_t*, image_t*, WORD` | Extracts layer |
| `OGRES_Free` | `ogres_t*`	| Releases resources |
| `OGRES_FreeImage` | `image_t*` | Releases image data |

---

## 📦 Related Tools

<img width="601" height="431" alt="image" src="https://github.com/user-attachments/assets/dc3673cf-f091-465c-8aa1-b65fdbd84cf2" />

* 🔨 [OGRES Writer GUI (Python)](https://github.com/gaetano-foster/OGRES-Writer) — Drag-and-drop tool for building `.ogres` files

---
## 🚧 Roadmap

    * Audio support (WAV/OGG chunks)
    * Zstandard compression
    * Linux/macOS ports
    * Unity/Unreal plugins
    * CLI inspection/debugging tools
    * Language bindings

---

## 🧠 License

MIT © 2025 Gaetano Foster
Use freely for both open-source and commercial projects.
