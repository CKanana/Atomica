# Atomica — From Orbits to Probability Clouds

**Group 5 · Quantum Visuals Inc. · Computer Graphics Semester Project**

A C++ / OpenGL desktop application that renders five historical and physical models of the **hydrogen atom** in sequence, so the user can see how our picture of "what an atom looks like" evolved, and why each earlier model was replaced.

## The five stages

| # | Stage | What it shows | Graphics techniques |
|---|-------|---------------|---------------------|
| 1 | Planetary | Electrons on fixed circular orbits (intuitive but wrong) | Circle / ellipse drawing algorithms, colour models |
| 2 | Bohr | Quantised energy levels, photon absorption and emission | Raster operations, frame-based animation |
| 3 | Standing Waves | Electrons as waves wrapped around the nucleus | Parametric curves, Bézier splines |
| 4 | 3D Probability Clouds | Electron position as a point cloud from hydrogen s and p wavefunctions | 3D transformations, homogeneous coordinates |
| 5 | Shaded Rendering | Lit, textured final visual | Lighting and shading models, texture mapping |

## Scope

- **In:** hydrogen atom only, using known closed-form wavefunctions for s and p orbitals; interactive, user-driven navigation between stages.
- **Out:** multi-electron atoms, live numerical solving of Schrödinger's equation, N-body electron dynamics, VR / AR / mobile / web.

## Technologies

- C++17
- OpenGL 3.3 Core
- GLFW (window and input), GLAD (OpenGL loader) and GLM (maths)
- CMake build system. GLFW and GLM are downloaded automatically on the first build; GLAD is committed under `external/glad`
- Tested with MinGW-w64 g++ (MSYS2 UCRT64) on Windows

## How to run

**Requirements:** Git, CMake 3.20 or newer, and a C++17 compiler (MinGW-w64 g++ on Windows). An internet connection is needed on the first build.

```bash
git clone https://github.com/CKanana/Atomica.git
cd Atomica
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/atomica.exe
```

**Controls:** keys `1` to `5` jump to a stage, the Left and Right arrow keys step through the stages, and `Esc` quits.

## Adding your stage

Each stage is a class that inherits from `Stage` (`src/core/Stage.h`). Put your class in your own stage folder, then register it in `main.cpp` in place of the matching `PlaceholderStage`. The stage manager handles switching, the render loop and key input.

## Team and responsibilities

| Member | GitHub | Responsibility |
|--------|--------|----------------|
| Crystal Kanana | @TODO | TODO |
| Donell Bikketi | @D0nell | Core + integration: project skeleton, render loop, stage manager, merging PRs |
| Emanuel Douglas | @TODO | TODO |
| Patrick Maina | @TODO | TODO |
| Gloria Kendi | @TODO | TODO |
| Andrew Kigondu | @TODO | TODO |

Suggested split: one lead for each of Stages 1-5, plus one member owning integration, UI, testing and documentation throughout. Everyone should understand the whole application and be able to explain their contribution.

## Timeline (12 weeks)

| Weeks | Milestone |
|-------|-----------|
| 1-2 | Planetary |
| 3-4 | Bohr |
| 5-6 | Standing Waves |
| 7-9 | 3D Probability Clouds |
| 10-11 | Shaded Rendering |
| 12 | Integration, testing, polish |

## Repository layout

```
Atomica/
├── src/
│   ├── core/            # window, main loop, stage manager, shared math, shader loader
│   ├── stage1_planetary/
│   ├── stage2_bohr/
│   ├── stage3_waves/
│   ├── stage4_clouds/
│   ├── stage5_shaded/
│   ├── ui/              # sidebar, info panel, stage navigation
│   └── main.cpp
├── external/
│   └── glad/            # OpenGL loader (generated, OpenGL 3.3 core)
├── shaders/
├── assets/
├── docs/                # technique write-ups, screenshots, design wireframe
├── CMakeLists.txt
├── README.md
└── CONTRIBUTING.md
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).