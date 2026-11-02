cmake_minimum_required(VERSION 3.20)
project(Atomica VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The vendored GLFW declares an old minimum CMake version. Newer CMake (4.x)
# refuses that unless told it is acceptable, so nobody needs a -D flag.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# ---------------------------------------------------------------- UI library
set(UI_SOURCES
    src/ui/Font8x8.h
    src/ui/Font8x8.cpp
    src/ui/Types.h
    src/ui/ICanvas.h
    src/ui/SoftwareCanvas.h
    src/ui/SoftwareCanvas.cpp
    src/ui/UIComponent.h
    src/ui/UIPanel.h
    src/ui/UIPanel.cpp
    src/ui/UIButton.h
    src/ui/UIButton.cpp
    src/ui/UISlider.h
    src/ui/UISlider.cpp
    src/ui/UIStageNavigator.h
    src/ui/UIStageNavigator.cpp
    src/ui/UIInfoPanel.h
    src/ui/UIInfoPanel.cpp
    src/ui/UIControlPanel.h
    src/ui/UIControlPanel.cpp
    src/ui/UILayoutManager.h
    src/ui/UILayoutManager.cpp
)
add_library(atomica_ui STATIC ${UI_SOURCES})
target_include_directories(atomica_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ui)

# Checkpoint 1 demo (software canvas only, writes a BMP)
add_executable(checkpoint1_demo src/ui/demo_checkpoint1.cpp)
target_link_libraries(checkpoint1_demo PRIVATE atomica_ui)

# ---------------------------------------------------------------- GLFW
# Built from the copy committed in external/, so no download and no
# library that only exists on one person's laptop.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw/glfw-3.3.8 EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------- Main app
# ONE target called atomica. Add new stage .cpp files to this list.
add_executable(atomica
    src/main.cpp
    src/core/StageManager.cpp
    src/stage1_planetary/PlanetaryStage.cpp
    src/stage2_bohr/BohrStage.cpp
    src/stage3_waves/StandingWavesStage.cpp
    src/stage4_clouds/ProbabilityCloudsStage.cpp
    src/stage5_shaded/ShadedRenderingStage.cpp
    glad_loader/glad.c
)
target_include_directories(atomica PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/glad_loader
)
target_link_libraries(atomica PRIVATE atomica_ui glfw)
if(WIN32)
    target_link_libraries(atomica PRIVATE opengl32 gdi32)
endif()
if(MINGW)
    target_link_options(atomica PRIVATE -static-libgcc -static-libstdc++)
endif()

# ---------------------------------------------------------------- Tests (off by default)
option(ATOMICA_BUILD_TESTS "Build the orbital unit test" OFF)
if(ATOMICA_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/orbital_test.cpp)
    enable_testing()
    add_executable(orbital_test tests/orbital_test.cpp src/stage4_clouds/Orbital.cpp)
    target_include_directories(orbital_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_test(NAME orbital_test COMMAND orbital_test)
endif()