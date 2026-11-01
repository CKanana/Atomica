cmake_minimum_required(VERSION 3.20)
project(Atomica VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UI Source Files
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

# UI Library
add_library(atomica_ui STATIC ${UI_SOURCES})
target_include_directories(atomica_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ui)

# Checkpoint 1 Demo Executable
add_executable(checkpoint1_demo src/ui/demo_checkpoint1.cpp)
target_link_libraries(checkpoint1_demo PRIVATE atomica_ui)

# Main Application Executable
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

# Explicitly tell the compiler where all headers are located
target_include_directories(atomica PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/glad_loader
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/glfw-3.3.8/include
)

# --- Link the UI library and the vendored glfw static library ---
# GLFW is built locally in external/glfw/build/src (libglfw3.a). Link against
# the full path so the MinGW linker resolves it; glad.c is compiled straight
# into the atomica executable.
target_link_libraries(atomica PRIVATE
    atomica_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/build/src/libglfw3.a
)
# --- Fetch GLFW automatically ---
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- Main Application Executable ---
add_executable(atomica 
    src/main.cpp
    src/core/StageManager.cpp
    src/stage1_planetary/PlanetaryStage.cpp
    external/glad/src/gl.c
    src/engine3d/Shader.cpp
    src/engine3d/CloudBuffer.cpp
    src/stage4_clouds/Orbital.cpp
    src/stage4_clouds/CloudStage.cpp
    src/stage5_shaded/ShadedStage.cpp
)

# Explicitly tell the compiler where all headers are located
target_include_directories(atomica PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${glfw_SOURCE_DIR}/include
)

# Link the UI library and GLFW
target_link_libraries(atomica PRIVATE atomica_ui glfw)

enable_testing()
add_executable(orbital_test tests/orbital_test.cpp src/stage4_clouds/Orbital.cpp)
target_include_directories(orbital_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME orbital_test COMMAND orbital_test)
