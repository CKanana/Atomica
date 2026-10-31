#pragma once
#include <glad/gl.h>

// Compile + link a vertex/fragment shader pair. Returns 0 (and prints the log) on error.
GLuint buildProgram(const char* vertexSrc, const char* fragmentSrc);
