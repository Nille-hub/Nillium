# Nillium

A Doom engine written in C++23 and OpenGL 4.6 core. It runs the original game
data of id Software's DOOM (1993) and DOOM II (1994) from a WAD file you own.
The goal is the plain vanilla Doom feel, plus resources from the Kex Remaster
(e.g. widescreen) and features of my own where desired.

## Background

Nillium is a deliberate rebuild of earlier versions of this engine, kept in a
private repository. Those versions worked, but hidden coupling made them hard
to change: shared mutable state with several writers, and an event bus used
for function calls in disguise.

The old code serves as reference only. Logic is ported module by module, and
only after it passes the architecture rules below. The rebuild is done with
Claude as reviewer and as a coach for software design.

## Design principles

- One writer per piece of mutable state.
- Dependencies point in one direction, through five layers:
  Platform, Assets, Game, Presentation, App.
- Events only where nothing breaks if nobody listens; everything else is a
  call, a return value, or state the reader checks.
- Data is passed as values instead of shared mutable objects.
- Fixed 35 Hz game tics, computed from absolute time and independent of the
  frame rate.
- Every GPU resource has exactly one RAII owner, which rules out both leaks
  and double deletion.
- Duplicated knowledge is removed; code that merely looks similar is not.

[`architecture.md`](architecture.md) is the map of the codebase: layers,
modules, the owner of each piece of state, and a dependency graph. It is
updated together with the code it describes.

## Status

- [x] Slice 1: window, GL context, fixed 35 Hz tic loop, clean shutdown
- [x] Slice 2: WAD access, palettes, first graphic on screen
- [x] Slice 3: title screen, menu and game mode
- [x] Slice 4: map loading
- [ ] Slice 5: world rendering
- [ ] Slice 6: thinkers, player, movement and collision
- [ ] Slice 7: sprites, HUD, weapons, automap, screen wipe
- [ ] Slice 8: audio

## Building

Requirements: Windows, Visual Studio 2022 with the C++ and CMake workloads.
GLFW is fetched automatically; glad is included.

    cmake --preset x64-debug
    cmake --build out/build/x64-debug

Or open the folder in Visual Studio and pick the `x64-debug` preset.

## Running

No game data is included. You need an IWAD from a copy of DOOM or DOOM II you
own, for example `DOOM.WAD` or `DOOM2.WAD`.

    out/build/x64-debug/bin/Nillium.exe -iwad path/to/DOOM2.WAD

## License

Copyright (C) 2026 Nille

Nillium is free software under the GNU General Public License v3.0 or later.
See [LICENSE](LICENSE).

## Disclaimer

DOOM is a registered trademark of id Software LLC, a ZeniMax Media company.
This project is not affiliated with or endorsed by id Software.
