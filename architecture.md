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

| Layer    | Module           | Responsibility                                                      |
|----------|------------------|---------------------------------------------------------------------|
| App      | `main.cpp`       | Entry point. Creates `App` and runs it.                             |
| App      | `app/app.*`      | Owns the platform objects. Runs the main loop: poll, tics, render.  |
| Platform | `platform/video.*` | Initialises and terminates GLFW. Owns the window and GL context.  |

## State ownership

| State             | Owner (writer)            | Readers | Layer    | Flow |
|-------------------|---------------------------|---------|----------|------|
| Window + GL context | `Video`                 | App     | Platform | App calls `video.swapBuffers()` and `video.pollEvents()`. |
| Close requested   | GLFW (OS close button), behind `Video` | App | Platform | App queries `video.closeRequested()`. |
| Quit decision     | App, local to the main loop | none  | App      | Combines all quit requests and ends the loop. |
| Framebuffer size  | GLFW, behind `Video`      | App     | Platform | App queries `video.framebufferSize()` and gets a value. |
| Last tic run      | App, local to the main loop | none  | App      | Computed from absolute elapsed time via `std::chrono::steady_clock`. |

## Events

None.

Rule: an event is legitimate only if nothing breaks when nobody listens.
Every event added here states that justification.

## Third-party dependencies

| Library | Version          | Source                                  | Used by                      |
|---------|------------------|-----------------------------------------|------------------------------|
| GLFW    | 3.4              | FetchContent, pinned commit             | Platform only                |
| glad    | 0.1.36, GL 4.6 core | Generated, vendored in `external/glad/` | Platform, Presentation, App |

- GLFW headers are included only in `platform/`. No GLFW type appears in a
  Platform public header.

## Conventions

### Naming
- No module suffixes: `Video`, `App`, not `VideoSystem` or `AppSys`.
- One class per file pair. The file is named after the class in lowercase:
  `platform/video.h` contains `class Video`.
- Files are named by concern, not by hardware category.
- Types in `PascalCase`, functions and variables in `camelCase`,
  data members with an `m_` prefix.

### Includes
- Project headers are included with the layer path and quotes:
  `#include "platform/video.h"`.
- A header from a higher layer in an include list is an architecture violation.

### GPU resources
- Every GL handle is created and deleted by exactly one move-only owner.
- GL resource owners are declared after `Video` in `App`, so they are destroyed
  before the GL context.