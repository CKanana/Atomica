// Atomica: entry point. Opens a window and runs the render loop.
#include <glad/gl.h>      // must come before GLFW
#include <GLFW/glfw3.h>

#include <cstdio>
#include <memory>
#include <string>

#include "core/PlaceholderStage.h"
#include "core/StageManager.h"
#include "stage1_planetary/PlanetaryStage.h"
#include "ui/SoftwareCanvas.h"
#include "stage4_clouds/CloudStage.h"
#include "stage5_shaded/ShadedStage.h"


static void onFramebufferResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

static void onKey(GLFWwindow* window, int key, int, int action, int) {
    auto* stages = static_cast<StageManager*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {   // 1-5 jump to a stage
            stages->switchTo(static_cast<std::size_t>(key - GLFW_KEY_1));
            return;
        }
        if (key == GLFW_KEY_RIGHT) { stages->next();     return; }
        if (key == GLFW_KEY_LEFT)  { stages->previous(); return; }
    }

    // Anything else goes to the active stage
    if (stages->current()) stages->current()->onKey(key, action);
}

int main() {
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to start GLFW\n");
        return 1;
    }

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

    {   // Inner scope: stages are destroyed while the OpenGL context is still alive
        
        // 1. Create the software canvas for your stage to draw on
        static atomica::ui::SoftwareCanvas sharedCanvas(1280, 720);
        
        StageManager stages;
        
        // 2. Replace the Planetary PlaceholderStage with your actual class
        stages.add(std::make_unique<PlanetaryStage>(&sharedCanvas));
        
        stages.add(std::make_unique<PlaceholderStage>("Bohr",            0.05f, 0.08f, 0.25f));
        stages.add(std::make_unique<PlaceholderStage>("Standing Waves",  0.05f, 0.20f, 0.10f));
        stages.add(std::make_unique<CloudStage>());
        stages.add(std::make_unique<ShadedStage>());
        

        glfwSetWindowUserPointer(window, &stages);
        glfwSetFramebufferSizeCallback(window, onFramebufferResize);
        glfwSetKeyCallback(window, onKey);

        int shownStage = -1;
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            double dt = now - lastTime;
            lastTime = now;

            // Show the active stage in the window title
            if (static_cast<int>(stages.currentIndex()) != shownStage) {
                shownStage = static_cast<int>(stages.currentIndex());
                std::string title = "Atomica - Stage " + std::to_string(shownStage + 1) +
                                    ": " + stages.current()->name();
                glfwSetWindowTitle(window, title.c_str());
            }

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            stages.current()->update(dt);
            stages.current()->render(width, height);

            static bool imageSaved = false;
            if (!imageSaved) {
                sharedCanvas.saveToBMP("planetary_test.bmp");
                imageSaved = true;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
