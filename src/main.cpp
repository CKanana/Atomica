// Atomica: entry point. Opens a window and runs the render loop.
#include <glad/gl.h>      // must come before GLFW
#include <GLFW/glfw3.h>

#include <cstdio>

static void onFramebufferResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

static void onKey(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to start GLFW\n");
        return 1;
    }

    // Ask for OpenGL 3.3 core, the same version we generated GLAD for
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Atomica", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to load OpenGL\n");
        glfwTerminate();
        return 1;
    }
    std::printf("OpenGL %s\n", glGetString(GL_VERSION));

    glfwSetFramebufferSizeCallback(window, onFramebufferResize);
    glfwSetKeyCallback(window, onKey);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.05f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}