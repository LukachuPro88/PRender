# Contributing to PRender

Thank you for taking the time to contribute! Because `PRender` is structured to keep graphics pipelines clean, lightweight, and renderer-agnostic where possible, we prioritize **predictability**, **performance**, and **strict architectural consistency**.

Please review the guidelines below to ensure your contributions align seamlessly with the rest of the library ecosystem.

---

## Architectural Principles

Before writing code, keep our core layout strategies in mind:

1. **Declarations Belong in the Header:** All public structs, typedefs, macros, `extern` globals, and function declarations must live in `prender.h`. Consumers should only ever need to include this single umbrella header.
2. **Hybrid Rendering Layers:** Use **raylib** for high-level, general-purpose operations. Reserve raw **OpenGL via GLAD** for anything that requires direct shader control — such as custom draw calls (`PR_DrawQuad`, `PR_DrawCircle`) or future shader-driven 3D rendering. Do not mix the two within the same function.
3. **Strict C11 Standards:** All contributions must comply with clean, standard C11 logic. Avoid compiler-specific extensions (like GCC or Clang specifics) to ensure cross-platform compiler viability.

---

## Development Workflow

### 1. Code Style Guidelines

- **General Style:** When in doubt, follow the patterns already established in the codebase — consistency across the library is a priority.
- **Naming Conventions:** All public structs, functions, macros, and global variables must be explicitly prefixed with `PR_` or `pr_` to prevent namespace collisions in external applications.
- **Color Values:** Colors are represented as `PR_Color` with normalized `float` channels (`0.0f`–`1.0f`). Predefined color macros follow the existing pattern using compound literals:

```c
#define PR_MY_COLOR (PR_Color){r, g, b, a}
```

- **Function Documentation:** Every new public function declaration must be preceded by a Doxygen-style comment block in `prender.h` outlining its behavior, parameter specs, and return attributes:

```c
/**
 * @brief Briefly describes what the function does.
 *
 * @param *window The window context.
 * @param position The position as PM_Vec2.
 * @return Description of the return value, if any.
 */
```

### 2. Testing Your Changes Locally

Before submitting your updates, make sure your changes don't break existing rendering behavior. Rebuild the library from your build directory to verify compilation:

```bash
cd build
cmake ..
make
```

### 3. Submitting a Pull Request

1. Fork the repository and create an isolated feature branch (`git checkout -b feature/amazing-new-renderer`).
2. Commit your changes with logical, descriptive messages (`git commit -m "Add PR_DrawLine primitive via GLAD shader pipeline"`).
3. Push your branch and open a Pull Request targeting the `main` branch.

We will review your rendering logic, shader implementations, and architectural decisions as fast as possible!
