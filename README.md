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

- C++
- OpenGL (course-specified graphics environment)
- _TODO: add libraries, compiler and build tool once confirmed (e.g. GLFW, GLAD, GLM, CMake)_

## How to run

_TODO: fill in once the project skeleton builds._

```bash
git clone <REPO_URL>
cd atomica
# build + run instructions here
```

## Team and responsibilities

| Member | GitHub | Responsibility |
|--------|--------|----------------|
| Crystal Kanana | @TODO | TODO |
| Donell Bikketi | @TODO | TODO |
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
atomica/
├── src/
│   ├── core/            # window, main loop, shared math, shader loader
│   ├── stage1_planetary/
│   ├── stage2_bohr/
│   ├── stage3_waves/
│   ├── stage4_clouds/
│   ├── stage5_shaded/
│   └── ui/              # sidebar, info panel, stage navigation
├── shaders/
├── assets/
├── docs/                # technique write-ups, screenshots, design wireframe
├── README.md
└── CONTRIBUTING.md
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).
