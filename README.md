# Atomica: From Orbits to Probability Clouds

**Group 5 | Quantum Visuals Inc. | Computer Graphics Semester Project**

Atomica is a C++ project about how scientific models of the hydrogen atom evolved. The project is planned as a sequence of five visual stages. The current checkpoint builds a software-rendered interface and a static atom preview using computer graphics algorithms implemented in C++.

## Current Checkpoint

The `checkpoint1_demo` executable draws a 1280 x 720 interface and hydrogen atom preview, then saves the result as `checkpoint1_ui_demo.bmp` in its working directory. It runs without an OpenGL context or third-party graphics libraries.

The new UI framework includes:

- A software canvas with line, rectangle, circle, ellipse, and cubic Bézier drawing
- Cohen-Sutherland line clipping and alpha blending
- An 8 x 8 bitmap font and BMP/PPM image export
- Reusable panels, buttons, sliders, stage navigation, and information/control panels
- A layout manager that coordinates the UI and central model viewport
- A demo scene showing planetary orbits, a de Broglie wave preview, and hydrogen atom annotations

The UI components expose mouse-event handling, but the current demo renders a single frame to an image rather than running as an interactive desktop window.

## Planned Atom Stages

These stages describe the project direction; their atom renderers are not part of the current checkpoint yet.

| # | Stage | Planned visualization | Graphics topics |
|---|-------|------------------------|-----------------|
| 1 | Planetary | Electrons on fixed circular orbits | Circle and ellipse algorithms, colour models |
| 2 | Bohr | Quantised energy levels and photon transitions | Raster operations, frame-based animation |
| 3 | Standing Waves | Matter waves around the nucleus | Parametric curves, Bézier splines |
| 4 | 3D Probability Clouds | Point clouds for hydrogen s and p orbitals | 3D transformations, homogeneous coordinates |
| 5 | Shaded Rendering | Lit and textured probability clouds | Lighting, shading, texture mapping |

## Technologies

- C++17
- CMake 3.20 or newer
- No external graphics dependencies are required for the current software-rendered checkpoint
- OpenGL is part of the planned application direction, but is not currently used by the build

## Build and Run

From the repository root, configure and build with CMake:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

With the Visual Studio CMake generator, run the demo with:

```powershell
.\build\Release\checkpoint1_demo.exe
```

With a single-configuration generator such as Ninja, the executable is typically at `build/checkpoint1_demo.exe`. The demo writes `checkpoint1_ui_demo.bmp` to the current working directory.

## Team and Responsibilities

| Member | GitHub | Responsibility |
|--------|--------|----------------|
| Crystal Kanana | @TODO | TODO |
| Donell Bikketi | @TODO | TODO |
| Emanuel Douglas | @TODO | TODO |
| Patrick Maina | @TODO | TODO |
| Gloria Kendi | @TODO | TODO |
| Andrew Kigondu | @TODO | TODO |

Suggested split: one lead for each of Stages 1-5, plus one member owning integration, UI, testing and documentation throughout. Everyone should understand the whole application and be able to explain their contribution.

## Timeline (12 Weeks)

| Weeks | Milestone |
|-------|-----------|
| 1-2 | Planetary |
| 3-4 | Bohr |
| 5-6 | Standing Waves |
| 7-9 | 3D Probability Clouds |
| 10-11 | Shaded Rendering |
| 12 | Integration, testing, polish |

## Repository Layout

```
atomica/
├── src/
│   ├── core/            # Planned shared application infrastructure
│   ├── stage1_planetary/ # Planned atom model stages
│   ├── stage2_bohr/
│   ├── stage3_waves/
│   ├── stage4_clouds/
│   ├── stage5_shaded/
│   └── ui/              # Software canvas, UI components, layout, and demo
├── shaders/             # Planned rendering assets
├── assets/              # Application assets
├── docs/                # Project documentation and design material
├── README.md
└── CONTRIBUTING.md
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).
