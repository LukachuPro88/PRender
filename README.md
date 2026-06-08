# PRender

**PRender** is a lightweight, custom C11 graphics abstraction layer designed for handling platform windows and rendering physical entities efficiently. It uses **raylib** for general-purpose rendering and **GLAD** for low-level OpenGL draw calls that require direct shader control — with planned support for **3D rendering** in a future release.

## Features

- **Window Context:** Quick platform window initialization wrapper around raylib's windowing system.
- **Hardware Accelerated Rendering:** Primitive draw calls such as `DrawQuad` and `DrawCircle` are implemented directly via GLAD, enabling full shader pipeline control where needed.
- **Hybrid Rendering Architecture:** Leverages raylib for high-level convenience and raw OpenGL via GLAD for performance-critical or shader-driven rendering paths.
- **3D Rendering _(planned)_:** Full 3D entity rendering support is on the roadmap and will be built on the existing GLAD shader infrastructure.

---

## How to Use

### 1. System Installation (Linux)

To build and install the library globally to your system directories, run the following from the project root:

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

### 2. Compilation & Linking

Link `PRender` and its dependencies in your CMake target configuration:

```cmake
target_link_libraries(your_executable_target PRIVATE prender pmath raylib m dl)
```

---

## License

This project is licensed under the terms of the MIT License.
# PRender

**PRender** is a lightweight, custom C11 graphics abstraction layer designed for handling platform windows and rendering physical entities efficiently. It uses **raylib** for general-purpose rendering and **GLAD** for low-level OpenGL draw calls that require direct shader control — with planned support for **3D rendering** in a future release.

> **PRender** is built on top of [PMath](https://github.com/LukachuPro88/PMath) and requires it as a core dependency. All spatial data — positions, sizes, and vectors — are expressed using `PM_Vec2` and other PMath primitives.

## Features

- **Window Context:** Quick platform window initialization wrapper around raylib's windowing system.
- **Hardware Accelerated Rendering:** Primitive draw calls such as `DrawQuad` and `DrawCircle` are implemented directly via GLAD, enabling full shader pipeline control where needed.
- **Hybrid Rendering Architecture:** Leverages raylib for high-level convenience and raw OpenGL via GLAD for performance-critical or shader-driven rendering paths.
- **3D Rendering _(planned)_:** Full 3D entity rendering support is on the roadmap and will be built on the existing GLAD shader infrastructure.

---

## How to Use

### 1. System Installation (Linux)

To build and install the library globally to your system directories, run the following from the project root:

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

### 2. Compilation & Linking

Link `PRender` and its dependencies in your CMake target configuration:

```cmake
target_link_libraries(your_executable_target PRIVATE prender pmath raylib m dl)
```

---

## License

This project is licensed under the terms of the MIT License.
