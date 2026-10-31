#!/bin/bash
set -o pipefail
ROOT="/c/Users/kanan/OneDrive/Desktop/Atomica"
BIN="/c/Users/kanan/Downloads/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-14.0.0-r7/mingw64/bin"
cd "$ROOT"
echo "=== CMAKE (regen) ==="
"$BIN/cmake" -S . -B build -G "MinGW Makefiles" > build/cmake.log 2>&1
echo "CMAKE_EXIT=$?"
tail -n 6 build/cmake.log
echo "=== MAKE ==="
"$BIN/mingw32-make" -j4 -C build > build/make.log 2>&1
echo "MAKE_EXIT=$?"
tail -n 40 build/make.log
echo "=== ARTIFACT ==="
ls -la build/atomica.exe 2>/dev/null && echo "ARTIFACT OK" || echo "NO ATOMICA.EXE"
