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
)

# Explicitly tell the compiler where all headers are located
target_include_directories(atomica PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${glfw_SOURCE_DIR}/include
)

# Link the UI library and GLFW
target_link_libraries(atomica PRIVATE atomica_ui glfw)