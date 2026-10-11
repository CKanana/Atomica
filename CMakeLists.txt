cmake_minimum_required(VERSION 3.20)
project(Atomica LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Lets older third-party libraries configure under newer CMake versions
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

# GLFW: window and input
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4)

# GLM: vectors and matrices
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1)

FetchContent_MakeAvailable(glfw glm)

# GLAD: OpenGL function loader (already in external/glad)
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

# Every .cpp file under src/ is picked up automatically
file(GLOB_RECURSE ATOMICA_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(atomica ${ATOMICA_SOURCES})
target_include_directories(atomica PRIVATE src)
target_link_libraries(atomica PRIVATE glad glfw glm::glm)

# MinGW: bundle the runtime so the .exe runs on any Windows laptop
if(MINGW)
    target_link_options(atomica PRIVATE -static)
endif()

# Copy shaders/ and assets/ next to the .exe after every build
add_custom_command(TARGET atomica POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:atomica>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:atomica>/assets)