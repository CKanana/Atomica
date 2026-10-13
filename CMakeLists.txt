cmake_minimum_required(VERSION 3.20)
project(Atomica VERSION 0.1.0 LANGUAGES CXX)

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
