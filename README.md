# Atomica: From Orbits to Probability Clouds

**Group 5 | Quantum Visuals Inc. | Computer Graphics Semester Project**

## 1. Project Overview

Atomica is a C++ computer graphics project that explores the evolution of scientific models of the hydrogen atom.

The application presents this evolution through five interactive visual stages, progressing from the classical planetary model to quantum mechanical representations of electron probability distributions.

The project uses OpenGL for graphics rendering, GLFW for window management and input handling, GLAD for OpenGL function loading, and a software canvas for the user interface.

## 2. Current Implementation

All five atom stage renderers have been implemented and integrated into the application's render loop.

| No. | Stage | Description |
|---|---|---|
| 1 | Planetary Model | Electrons moving along fixed circular orbits around a nucleus, with glow effects. |
| 2 | Bohr Model | Quantised electron energy levels and photon jump transitions. |
| 3 | Standing Waves | de Broglie standing-wave lobes using Bézier splines and nodes. |
| 4 | Probability Clouds | Monte Carlo-generated electron probability distributions based on \(|\psi|^2\), with adjustable density. |
| 5 | Shaded Rendering | Lit spherical probability clouds using Lambertian shading. |

### Application Architecture

The application opens a native Windows window and renders the active stage into a shared software canvas.

The canvas is uploaded to an OpenGL texture, which is displayed using a full-screen textured quad each frame.

The central viewport contains the active stage renderer, while the side panels and stage navigator are drawn over the interface.

## 3. Technologies and Requirements

### Technologies

- **C++17** – Primary programming language.
- **OpenGL 3.3 Core** – Graphics rendering.
- **GLFW 3.3.8** – Window creation and input handling.
- **GLAD** – OpenGL function loading.
- **CMake** – Build system.

### Development Environment

The project is developed and tested on Windows using MinGW-w64 through MSYS2 UCRT64.

### Requirements

- Git
- CMake 3.20 or newer
- MinGW-w64 C++ compiler
- MSYS2 UCRT64 development environment (recommended)

The project includes a vendored GLFW source directory, so GLFW does not need to be installed separately.

## 4. Installation and Build Instructions

### Step 1: Clone the repository

```bash
git clone https://github.com/CKanana/Atomica.git
cd Atomica
```

### Step 2: Configure the project

Open the MSYS2 UCRT64 terminal from the repository root and run:

```bash
cmake -S . -B build2 -G "MinGW Makefiles"
```

### Step 3: Build the application

```bash
cmake --build build2 -j4
```

### Step 4: Run Atomica

From MSYS2 UCRT64:

```bash
./build2/atomica.exe
```

Alternatively, from Windows PowerShell:

```powershell
.\build2\atomica.exe
```

The application opens in a native window with a resolution of 1280 × 720 pixels.

### Build Outputs

The CMake configuration defines two executable targets:

- `atomica.exe` – Main Atomica application.
- `checkpoint1_demo.exe` – Software canvas demonstration.

## 5. Application Controls

| Key / Input | Action |
|---|---|
| `1` – `5` | Switch between the five stages. |
| `Left Arrow` | Move to the previous stage. |
| `Right Arrow` | Move to the next stage. |
| `Space` | Play or pause the active stage. |
| `Esc` | Close the application. |
| Mouse | Interact with the central viewport and UI controls. |

Additional stage controls are available through the side panels. Depending on the active stage, these include speed, radius, quantum level, wave count, point density, photon transitions and reset controls.

## 6. Repository Structure

```text
Atomica/
├── src/
│   ├── core/
│   │   ├── Animation.h
│   │   ├── PlaceholderStage.h
│   │   ├── Stage.h
│   │   ├── StageManager.h
│   │   └── StageManager.cpp
│   │
│   ├── engine3d/
│   │   ├── CloudBuffer.cpp
│   │   ├── CloudBuffer.h
│   │   ├── Mat4.h
│   │   ├── OrbitCamera.h
│   │   ├── Shader.cpp
│   │   └── Shader.h
│   │
│   ├── stage1_planetary/
│   │   ├── PlanetaryStage.cpp
│   │   └── PlanetaryStage.h
│   │
│   ├── stage2_bohr/
│   │   ├── BohrStage.cpp
│   │   └── BohrStage.h
│   │
│   ├── stage3_waves/
│   │   ├── StandingWavesStage.cpp
│   │   └── StandingWavesStage.h
│   │
│   ├── stage4_clouds/
│   │   ├── CloudStage.*
│   │   ├── Orbital.*
│   │   ├── OrbitalPresets.h
│   │   └── ProbabilityCloudsStage.*
│   │
│   ├── stage5_shaded/
│   │   ├── DensityRamp.h
│   │   ├── ShadedRenderingStage.*
│   │   └── ShadedStage.*
│   │
│   ├── ui/
│   │   ├── Font8x8.*
│   │   ├── ICanvas.h
│   │   ├── SoftwareCanvas.*
│   │   ├── UIComponent.h
│   │   ├── UIPanel.*
│   │   ├── UIButton.*
│   │   ├── UISlider.*
│   │   ├── UIStageNavigator.*
│   │   ├── UIInfoPanel.*
│   │   ├── UIControlPanel.*
│   │   ├── UILayoutManager.*
│   │   └── Types.h
│   │
│   └── main.cpp
│
├── external/
│   ├── glad/
│   └── glfw/
│       └── glfw-3.3.8/
│
├── glad_loader/
│   ├── glad.c
│   ├── glad.h
│   └── khrplatform.h
│
├── CMakeLists.txt
├── README.md
└── CONTRIBUTING.md
```

**Note:** The main application currently compiles GLAD from `glad_loader/glad.c`. The additional `external/glad/` directory is also present in the repository.

## 7. Adding a New Stage

Each atom stage is implemented as a class that inherits from the shared `Stage` interface located in:

```text
src/core/Stage.h
```

To add or modify a stage:

1. Create or update the relevant stage class in its corresponding stage directory.
2. Implement the required stage interface methods.
3. Register the stage with the application through the stage management and integration code.
4. Test the stage to ensure rendering, controls and stage transitions work correctly.

The `StageManager` is responsible for managing the active stage and switching between stages.

## 8. Team Members and Responsibilities

Atomica is developed by Group 5 – Quantum Visuals Inc. Each member is assigned responsibility for a specific component of the project.

| Member | GitHub | Responsibility |
|---|---|---|
| Crystal Kanana | [@CKanana](https://github.com/CKanana) | UI Layout – sidebar, viewport, information panel and buttons |
| Donell Bikketi | [@D0nell](https://github.com/D0nell) | Core and Integration – project skeleton, render loop, stage manager and pull request integration |
| Emanuel Douglas | [@DiggaDouglas](https://github.com/DiggaDouglas) | Circles and Atoms – nucleus, electrons and orbital representations |
| Patrick Maina | [@mainanp](https://github.com/mainanp) | Animation |
| Gloria Kendi | [@gkendi2](https://github.com/gkendi2) | Theme and Documentation – colours, styling guide, README and screenshots |
| Andrew Kigondu | [@Kigondinho](https://github.com/Kigondinho) | Lines and Polygons |

## 9. Project Timeline

| Weeks | Milestone |
|---|---|
| 1–2 | Planetary Model |
| 3–4 | Bohr Model |
| 5–6 | Standing Waves |
| 7–9 | 3D Probability Clouds |
| 10–11 | Shaded Rendering |
| 12 | Integration, Testing and Polish |

## 10. Contributing

Atomica is developed collaboratively by Group 5.

Team members should follow the project's contribution workflow when creating branches, implementing changes and submitting pull requests.

For branch management, commit-message conventions and contribution guidelines, see [CONTRIBUTING.md](CONTRIBUTING.md).

---

**Group 5 | Quantum Visuals Inc.**

*Computer Graphics Semester Project*