# Minimal C++ Image Editor

A small standalone C++17 image editor built on OpenCV's HighGUI. Not part of the
surrounding web project — build and run it locally.

## Features

- Gaussian blur (slider)
- Brightness / contrast (sliders)
- Saturation (slider, HSV-based)
- Negate / invert colors (toggle)
- Grayscale (toggle)
- Sepia (toggle, 3×3 color matrix)
- Free-angle rotate (slider, canvas expands) + 90° CW commit (`r`)
- Crop (mouse drag + Enter)
- Resize: scale slider (10%–200%) + exact WxH (`R`)
- Undo stack (`u`, depth 20)

Slider adjustments are non-destructive. Commit them into the base image with
`C`, then keep editing.

## Install OpenCV

- **macOS:** `brew install opencv`
- **Ubuntu / Debian:** `sudo apt install libopencv-dev`
- **Fedora:** `sudo dnf install opencv-devel`
- **Windows (vcpkg):** `vcpkg install opencv4` (then pass `-DCMAKE_TOOLCHAIN_FILE=...` to `cmake`)

## Build

```bash
cd cpp-image-editor
cmake -B build
cmake --build build -j
```

## Run

```bash
./build/image_editor path/to/photo.jpg
# or with no arg — you'll be prompted for a path
./build/image_editor
```

## Keyboard

| Key       | Action                                              |
|-----------|-----------------------------------------------------|
| `o`       | Open a new image (path prompted in terminal)        |
| `w`       | Save current view (path prompted)                   |
| `u`       | Undo last destructive op                            |
| `n`       | Toggle negate                                       |
| `g`       | Toggle grayscale                                    |
| `s`       | Toggle sepia                                        |
| `r`       | Rotate 90° CW (committed)                           |
| `c`       | Enter crop mode — drag mouse, `Enter` apply, `Esc` cancel |
| `R`       | Resize to exact WxH (e.g. `800x600`)                |
| `C`       | Commit current slider state into base               |
| `h`       | Print help                                          |
| `q`/`Esc` | Quit                                                |

## Layout

```
cpp-image-editor/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp        event loop + hotkeys
    ├── ops.{h,cpp}     pure image operations
    ├── ui.{h,cpp}      window, trackbars, mouse, undo
    └── io.{h,cpp}      load/save + terminal prompts
```