# Build, setup, and generated-loader recipe

## The repo comes out broken — restore these before any target compiles

1. `src/main.cpp` — tracked in git but deleted in the working tree (blob `4c11a37` is the
   original). Restore:

       git show 4c11a37:src/main.cpp > src/main.cpp

2. `src/core/StageManager.cpp` — present in the working tree but never committed to git
   (it exists in the historical commit `f881d46`). Restore:

       git show f881d46:src/core/StageManager.cpp > src/core/StageManager.cpp

   (If it is missing in a future checkout, restore it the same way.)

## Working CMake + MinGW build

From the repo root:

    rm -rf build && mkdir build
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    cmake --build build -j<N>

`CMAKE_POLICY_VERSION_MINIMUM=3.5` is required because glad's `CMakeLists.txt` (CMake 3.0
minimum) fails on CMake 4.3. The winlibs CMake 4.3 binary is at
`C:\Users\kanan\Downloads\winlibs-*.exe`;
if the MSYS `cmake` on PATH is older and shadows it, invoke the winlibs binary by path.

MSYS path note: native Windows compilers reject MSYS-style paths (`/tmp/foo.cpp`) even
though bash `cd /tmp` works. Pass native `C:\...` or `C:/Users/...` forward slashes to the
winlibs binaries. For scratch files a native tool must read, use `%LOCALAPPDATA%\Temp`.

## Generated GL loader for the `atomica` target

`src/main.cpp` includes `<glad/glad.h>` (not `<glad/gl.h>`). The loader is vendored under
`glad_loader/` and generated on the machine (committed) using the Glad CLI from
`glad_src/glad-master/glad/__main__.py`:

    python -m glad.__main__ --profile compatibility --api gl=3.3 \
        --generator c --local-files --quiet --out-path glad_loader

Requirements (this environment is picky):

- **Do NOT pass `--omit-khrplatform` or `--no-loader`**: the generated `glad.h` then lacks
  `KHR/khrplatform.h` and fails to compile with `unknown type name 'khronos_float_t'`
  (and friends). Keep the loader's `KHR` typedefs.
- **Keep `--local-files`** so it is fully self-contained and works offline.
- `glad.h` includes `"khrplatform.h"` by name, so `khrplatform.h` must sit in the same
  directory as `glad.h` (i.e. `glad_loader/`, not a `KHR/` subfolder). The CLI generates a
  `KHR/` subfolder, so copy `khrplatform.h` up one level after generation.
- `main.cpp` and the stages use the R1 interface from `Stage.h`. The `atomica` target
  compiles `src/main.cpp`, `src/core/StageManager.cpp`,
  `src/stage1_planetary/PlanetaryStage.cpp`, and `glad_loader/glad.c`.

## What to check before calling a build "done"

- `build/atomica.exe` exists and runs without an OpenGL/GLFW error.
- Every stage reaches its renderer (jump keys 1-5, Left/Right, navigator click, Play/Pause,
  sliders, Photon Jump; F12 saves `atomica_preview.bmp`).
- A clean rebuild from scratch produces `atomica.exe` (no stale `atomica.dir` object sets).

## MSYS directory lock during builds (durable workaround)

When a build complains 'rm: cannot remove ... Device or resource busy' (MSYS holds the
directory open between configure/make calls), do NOT re-run plain `rm -rf build`. Instead:

    cmd /c "rmdir /s /q build"   # Windows, bypasses the MSYS lock
    mkdir build

Then re-run cmake (from the repo root) and mingw32-make. A *clean* rebuild is the only
way to be sure of a green tree: `CMakeFiles/atomica.dir` objects get stale and the
`atomica.exe` target can silently skip compiling `src/main.cpp`, producing a 'No targets
specified' error even when the source on disk is correct. If errors mention 'layout is not
captured' or 'struct {anonymous}::InputContext has no member named' against a file that
visually contains those names, the object set is stale — wipe the build dir and rebuild
clean before touching code again.

### Confirming a true clean rebuild (not a stale target)

- `CMakeFiles/atomica.dir/src/main.cpp.obj` must be (re)created from the current
  `src/main.cpp`; grep the compile line in `build/CMakeFiles/atomica.dir/build.make`.
- `build/atomica.exe` must be (re)linked after the last change.

### Windows/MinGW path notes

- Native Windows compilers reject MSYS-style paths. Pass native `C:\...` or
  `C:/Users/...` forward slashes to the winlibs binaries. For scratch files a native
  tool must read, use `%LOCALAPPDATA%\Temp`.
- The winlibs CMake 4.3 binary is at
  `C:\Users\kanan\Downloads\winlibs-*.exe`; if the MSYS `cmake` on PATH is older and
  shadows it, invoke the winlibs binary by path.
