# Nillium Architecture

This file is the map of the codebase: layers, modules, owners of state, and events.
It is updated in the same commit series as the change it describes.

## Layers

Dependencies point downward only. A lower layer never calls, includes or subscribes
to a higher one. Upward information travels as return values, as state the higher
layer reads, or as a fire-and-forget event.

| # | Layer        | Folder              | Contains                                                    |
|---|--------------|---------------------|-------------------------------------------------------------|
| 5 | App          | `src/app/`, `src/main.cpp` | Main loop and wiring. The only layer that knows all others. |
| 4 | Presentation | `src/presentation/` | Renderers, HUD, menu drawing, audio playback. Reads game state, never writes it. |
| 3 | Game         | `src/game/`         | Game state and tic-based simulation. No GL, no rendering types. |
| 2 | Assets       | `src/assets/`       | WAD access and decoded resources. Immutable after loading.  |
| 1 | Platform     | `src/platform/`     | Window, GL context and loader, input devices, audio device. |

Folders are created when the first module of that layer is added.

## Modules

| Layer        | Module                        | Responsibility |
|--------------|-------------------------------|----------------|
| App          | `main.cpp`                    | Entry point. Parses `-iwad`, creates `App` and runs it. |
| App          | `app/app.*`                   | Owns the top-level objects (`Video`, `Wad`, renderers). Runs the main loop: poll, tics, render. |
| Presentation | `presentation/page.*`         | Draws a full-screen 320x200 graphic centred at 4:3. Applies the palette in its shader. |
| Presentation | `presentation/texture.*`      | RAII owner of one GL texture. |
| Presentation | `presentation/shaderprogram.*` | RAII owner of one GL shader program. |
| Presentation | `presentation/vertexarray.*`  | RAII owner of one GL vertex array object. |
| Assets       | `assets/wad.*`                | Reads a WAD file once. Owns its bytes and directory. Lump lookup by name, last match wins. |
| Assets       | `assets/graphic.*`            | Decodes a patch lump into a `WadGraphic`: palette indices and opaque flags, row-major. |
| Assets       | `assets/palette.*`            | Decodes the 14 palettes from `PLAYPAL`. |
| Platform     | `platform/video.*`            | Initialises and terminates GLFW. Owns the window and GL context. |

## State ownership

| State             | Owner (writer)            | Readers | Layer    | Flow |
|-------------------|---------------------------|---------|----------|------|
| Window + GL context | `Video`                 | App     | Platform | App calls `video.swapBuffers()` and `video.pollEvents()`. |
| Close requested   | GLFW (OS close button), behind `Video` | App | Platform | App queries `video.closeRequested()`. |
| Quit decision     | App, local to the main loop | none  | App      | Combines all quit requests and ends the loop. |
| Framebuffer size  | GLFW, behind `Video`      | App     | Platform | App queries `video.framebufferSize()` and passes the value to renderers. |
| Last tic run      | App, local to the main loop | none  | App      | Computed from absolute elapsed time via `std::chrono::steady_clock`. |
| WAD path          | `main`, parsed once from `-iwad` | App | App   | Passed to App's constructor as a value. Missing argument: usage, exit code 1. |
| WAD data (bytes + directory) | `Wad`, written once in its constructor | Assets functions, via `const Wad&` | Assets | App constructs `Wad` from the path. |
| Palettes (14, from `PLAYPAL`) | Written once by `readPalettes` | `Page` constructor | Assets | Returned as a value; uploaded once as the palette texture. |
| `WadGraphic`      | Written once by `readGraphic` | `Page` constructor | Assets | Returned as a value; uploaded once as the graphic texture. |
| `Page` GL resources (graphic texture, palette texture, shader program, vertex array) | RAII owners inside `Page`, immutable after construction | `Page`, when drawing | Presentation | The shader looks up each pixel's colour in palette row 0. |
| `Page`            | App member, declared after `Video` | App | Presentation | App calls `page.draw(framebufferSize)` each frame. |

## Events

None.

Rule: an event is legitimate only if nothing breaks when nobody listens.
Every event added here states that justification.

## Third-party dependencies

| Library | Version          | Source                                  | Used by                      |
|---------|------------------|-----------------------------------------|------------------------------|
| GLFW    | 3.4              | FetchContent, pinned commit             | Platform only                |
| glad    | 0.1.36, GL 4.6 core | Generated, vendored in `external/glad/` | Platform, Presentation |

- GLFW headers are included only in `platform/` source files. No public member
  function of a Platform class takes or returns a GLFW type; a forward
  declaration for a private member is allowed.

## Conventions

### Naming
- No module suffixes: `Video`, `App`, not `VideoSystem` or `AppSys`.
- One class per file pair. The file is named after the class in lowercase:
  `platform/video.h` contains `class Video`.
- Files are named by concern, not by hardware category.
- Types in `PascalCase`, functions and variables in `camelCase`,
  data members with an `m_` prefix.
- `*` and `&` bind to the type: `GLFWwindow* window`, `const Video& video`.

### Includes
- Project headers are included with the layer path and quotes:
  `#include "platform/video.h"`.
- A header from a higher layer in an include list is an architecture violation.

### Assets
- Decoders are free functions `readX(const Wad&, ...)` that return plain structs.
- Decoded assets are immutable after loading. They hold palette indices,
  never colours; the palette is applied when drawing.

### GPU resources
- Every GL handle is created and deleted by exactly one move-only owner.
- GL resource owners are declared after `Video` in `App`, so they are destroyed
  before the GL context.
- Shader sources are embedded in the `.cpp` of the module that uses them.
