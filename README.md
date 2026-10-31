# Atomica: From Orbits to Probability Clouds

**Group 5 | Quantum Visuals Inc. | Computer Graphics Semester Project**

Atomica is a C++ project about how scientific models of the hydrogen atom evolved.
It is planned as a sequence of five visual stages, each an interactive
OpenGL app built with GLFW, Glad, and a software canvas for the UI.

## Current state

All five atom stage renderers are implemented and wired into the render loop:

| # | Stage | Renderer |
|---|-------|----------|
| 1 | Planetary | Electrons on fixed circular orbits (nucleus + glow) |
| 2 | Bohr | Quantised energy levels + photon jump transition |
| 3 | Standing Waves | de Broglie standing-wave lobes (Bézier splines) + nodes |
| 4 | Probability Clouds | Monte-Carlo `|ψ|²` point cloud, density-tunable |
| 5 | Shaded Rendering | Lit spherical-cloud volume with Lambertian shading |

The app opens a native Windows window (no DISPLAY/X11 needed). It renders the
active stage into a shared software canvas, uploads it to a GL texture, and
draws one full-screen textured quad each frame. The central viewport is the
stage renderer's clip region; the side panels and navigator draw on top.

## Build and run

From the repository root. The project targets MinGW-w64 (posix-seh); the winlibs
toolchain is expected at `C:/Users/kanan/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-14.0.0-r7/`.

```powershell
# 1. Configure with the MinGW Makefiles generator
cmake -S . -B build2 -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# 2. Build (always in a fresh folder; never rm -rf build)
mingw32-make -C build2
```

This produces `build2/atomica.exe`. The build requires:
- `cmake` and `g++` on `PATH`
- GLFW built from source in `external/glfw/build/` (static `libglfw3.a`)
- A vendored Glad loader (`glad_loader/glad.c` / `.h`)

Run the app:

```powershell
cd C:\Users\kanan\OneDrive\Desktop\Atomica
.\build2\atomica.exe
```

The 1280×720 window opens with the render loop running.

## Controls

| Key | Action |
|-----|--------|
| `1`–`5` | Switch between the five stages |
| `Left` / `Right` | Next / previous stage |
| `Space` | Play / pause the active stage |
| `Esc` | Close the window |

Mouse: click/drag on the central viewport; the stage navigator and play/pause
button are in the side panels. Each stage exposes controls via the side panels
(speed, radius, quantum level, wave count, point density, photon jump, reset).

## Repository layout

```
atomica/
├── src/
│   ├── core/                 # Stage interface + StageManager
│   ├── stage1_planetary/     # PlanetaryStage (circular orbits)
│   ├── stage2_bohr/          # BohrStage (energy levels + photon jump)
│   ├── stage3_waves/         # StandingWavesStage (Bézier lobes)
│   ├── stage4_clouds/        # ProbabilityCloudsStage (Monte-Carlo)
│   ├── stage5_shaded/        # ShadedRenderingStage (Lambertian)
│   └── ui/                   # SoftwareCanvas, UI components, layout
├── external/glfw/            # Vendored GLFW, built into external/glfw/build/
├── glad_loader/              # Vendored Glad loader
├── build2/                   # Build output (atomica.exe)
├── README.md
└── CONTRIBUTING.md
```

## Team

Implemented by the team. See [CONTRIBUTING.md](CONTRIBUTING.md) for the
contribution workflow and commit-message conventions.

## Timeline

| Weeks | Milestone |
|-------|-----------|
| 1-2 | Planetary |
| 3-4 | Bohr |
| 5-6 | Standing Waves |
| 7-9 | 3D Probability Clouds |
| 10-11 | Shaded Rendering |
| 12 | Integration, testing, polish |
