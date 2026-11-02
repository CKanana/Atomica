// Atomica: entry point. Opens a window and runs the render loop.
#include <glad/glad.h>      // must come before GLFW
#include <GLFW/glfw3.h>

#include <cstdio>
#include <memory>
#include <string>

#include "core/StageManager.h"
#include "core/Stage.h"
#include "core/PlaceholderStage.h"
#include "stage1_planetary/PlanetaryStage.h"
#include "stage2_bohr/BohrStage.h"
#include "stage3_waves/StandingWavesStage.h"
#include "stage4_clouds/ProbabilityCloudsStage.h"
#include "stage5_shaded/ShadedRenderingStage.h"
#include "ui/SoftwareCanvas.h"
#include "ui/UILayoutManager.h"

namespace {

// --- Full-screen textured-quad renderer (built once, alive for the whole run) ---
struct QuadRenderer {
    GLuint program = 0;
    GLuint vao = 0;

    bool init() {
        const char* VS_SOURCE =
            "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "out vec2 vTex;\n"
            "void main() {\n"
            "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
            "    vTex = aPos * 0.5 + 0.5;\n"
            "}\n";
        const char* FS_SOURCE =
            "#version 330 core\n"
            "in vec2 vTex;\n"
            "out vec4 outColor;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    outColor = texture(uTex, vec2(vTex.x, 1.0 - vTex.y));\n"
            "}\n";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &VS_SOURCE, nullptr);
        glCompileShader(vs);
        {
            GLint ok = 0;
            glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                GLint len = 0;
                glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
                std::vector<char> log(static_cast<size_t>(len));
                glGetShaderInfoLog(vs, len, nullptr, log.data());
                std::fprintf(stderr, "Vertex shader compile error: %s\n", log.data());
                glDeleteShader(vs);
                return false;
            }
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &FS_SOURCE, nullptr);
        glCompileShader(fs);
        {
            GLint ok = 0;
            glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                GLint len = 0;
                glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
                std::vector<char> log(static_cast<size_t>(len));
                glGetShaderInfoLog(fs, len, nullptr, log.data());
                std::fprintf(stderr, "Fragment shader compile error: %s\n", log.data());
                glDeleteShader(vs);
                glDeleteShader(fs);
                return false;
            }
        }

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        {
            GLint ok = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &ok);
            if (!ok) {
                GLint len = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
                std::vector<char> log(static_cast<size_t>(len));
                glGetProgramInfoLog(program, len, nullptr, log.data());
                std::fprintf(stderr, "Program link error: %s\n", log.data());
                glDeleteProgram(program);
                glDeleteShader(vs);
                glDeleteShader(fs);
                return false;
            }
        }
        glDeleteShader(vs);
        glDeleteShader(fs);

        glGenVertexArrays(1, &vao);
        float quad[6 * 2] = {
            -1.0f, -1.0f,   // 0 bottom-left
            -1.0f,  1.0f,   // 1 top-left
            1.0f,  -1.0f,   // 2 bottom-right
            1.0f,  -1.0f,   // 2 bottom-right
            -1.0f,  1.0f,   // 1 top-left
            1.0f,   1.0f    // 3 top-right
        };
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        return true;
    }

    void render(GLuint tex) const {
        glUseProgram(program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }
};

} // anonymous namespace

// --- GLFW callback helpers ---
// GLFW requires function pointers (non-capturing lambdas cannot be passed to
// GLFW). The standard C data-pointer pattern: an InputContext is stored in the
// window's user pointer and read out by the static callback helpers.
// These callbacks are registered only AFTER the three objects (stages, layout,
// canvas) are constructed and the context pointer is set.

struct InputContext {
    StageManager* stages = nullptr;
    atomica::ui::UILayoutManager* layout = nullptr;
    atomica::ui::SoftwareCanvas* canvas = nullptr;
};

void onFramebufferResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void onKey(GLFWwindow* w, int key_, int, int action, int mods) {
    auto* ctx = static_cast<InputContext*>(glfwGetWindowUserPointer(w));
    StageManager* stages = ctx->stages;

    if (action == GLFW_PRESS) {
        if (key_ == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
            return;
        }
        if (key_ >= GLFW_KEY_1 && key_ <= GLFW_KEY_5) {
            stages->switchTo(static_cast<std::size_t>(key_ - GLFW_KEY_1));
            return;
        }
        if (key_ == GLFW_KEY_LEFT)  { stages->previous(); return; }
        if (key_ == GLFW_KEY_RIGHT) { stages->next();     return; }
        if (key_ == GLFW_KEY_SPACE) {
            if (stages->current()) stages->current()->setPaused(!stages->current()->getPaused());
            return;
        }
    }
    if (stages->current()) stages->current()->onKey(key_, action);
}

void onMouseMove(GLFWwindow* w, double x, double y) {
    auto* ctx = static_cast<InputContext*>(glfwGetWindowUserPointer(w));
    StageManager* stages = ctx->stages;
    atomica::ui::UILayoutManager* layout = ctx->layout;
    int wx = static_cast<int>(x);
    int wy = static_cast<int>(y);
    int ww, wh;
    glfwGetWindowSize(w, &ww, &wh);
    if (stages->current() && layout->getViewportBounds().contains(wx, wy)) {
        // Clamp to canvas size to avoid out-of-range access.
        wx = std::clamp(wx, 0, ctx->canvas->getWidth());
        wy = std::clamp(wy, 0, ctx->canvas->getHeight());
    }
    if (layout->handleMouseMove(wx, wy)) return;
    if (stages->current()) stages->current()->onKey(GLFW_MOUSE_BUTTON_1, GLFW_PRESS);
}

void onMouseButton(GLFWwindow* w, int button, int action, int mods) {
    auto* ctx = static_cast<InputContext*>(glfwGetWindowUserPointer(w));
    StageManager* stages = ctx->stages;
    atomica::ui::UILayoutManager* layout = ctx->layout;
    int wx, wy;
    glfwGetCursorPos(w, reinterpret_cast<double*>(&wx), reinterpret_cast<double*>(&wy));
    int ww, wh;
    glfwGetWindowSize(w, &ww, &wh);
    wx = std::clamp(wx, 0, ww - 1);
    wy = std::clamp(wy, 0, wh - 1);

    if (action == GLFW_PRESS) {
        if (layout->handleMouseDown(wx, wy)) {
            if (stages->current()) stages->current()->onKey(button, GLFW_PRESS);
            return;
        }
        if (stages->current()) stages->current()->onKey(button, GLFW_PRESS);
    } else if (action == GLFW_RELEASE) {
        if (layout->handleMouseUp(wx, wy)) {
            if (stages->current()) stages->current()->onKey(button, GLFW_RELEASE);
            return;
        }
        if (stages->current()) stages->current()->onKey(button, GLFW_RELEASE);
    }
}

void onCursorPos(GLFWwindow* w, double x, double y) {
    auto* ctx = static_cast<InputContext*>(glfwGetWindowUserPointer(w));
    (void)w; (void)x; (void)y;
    (void)ctx->stages; (void)ctx->layout;
}

void onScroll(GLFWwindow* w, double xoff, double yoff) {
    auto* ctx = static_cast<InputContext*>(glfwGetWindowUserPointer(w));
    StageManager* stages = ctx->stages;
    if (stages->current()) {
        stages->current()->onScroll(yoff);
    }
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

    // Load GL function pointers via GLFW's proc address (MinGW builds need the
    // glad loader cast; glad.c exposes gladLoadGLLoader for this).
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::fprintf(stderr, "Failed to load OpenGL function pointers\n");
        glfwTerminate();
        return 1;
    }
    std::printf("OpenGL %s\n", glGetString(GL_VERSION));

    // --- Full-screen textured-quad renderer (built once, alive for the whole run) ---
    QuadRenderer quad;
    if (!quad.init()) {
        std::fprintf(stderr, "Failed to initialize full-screen quad renderer\n");
        glfwTerminate();
        return 1;
    }

    // --- Shared software canvas for the UI layout + the active stage ---
    atomica::ui::SoftwareCanvas canvas(1280, 720);

    // --- Stage manager + layout ---
    StageManager stages;
    stages.add(std::make_unique<PlanetaryStage>(&canvas));
    stages.add(std::make_unique<BohrStage>(&canvas));
    stages.add(std::make_unique<StandingWavesStage>(&canvas));
    stages.add(std::make_unique<ProbabilityCloudsStage>(&canvas));
    stages.add(std::make_unique<ShadedRenderingStage>(&canvas));

    atomica::ui::UILayoutManager layout(1280, 720);

    // Input context: passed as the GLFW user pointer BEFORE any callback is
    // registered, so static callbacks can reach main() objects.
    InputContext ctx;
    ctx.stages = &stages;
    ctx.layout = &layout;
    ctx.canvas = &canvas;
    glfwSetWindowUserPointer(window, &ctx);

    // Input callbacks registered AFTER the three objects (stages, layout,
    // canvas) are constructed and the context pointer is set.
    glfwSetFramebufferSizeCallback(window, onFramebufferResize);
    glfwSetKeyCallback(window, onKey);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onCursorPos);
    glfwSetScrollCallback(window, onScroll);

    // --- UI wiring: wire the ControlPanel and Navigator callbacks ---
    // Every lambda that touches layout or stages captures [&stages, &layout]
    // explicitly, per the project convention. stages is a value, so use '.'
    // on it and '->' only on pointers (stages.current() returns Stage*).
    layout.getControlPanel()->setOnSpeedChanged([&stages](float s) {
        if (stages.current()) stages.current()->setSpeed(s);
    });
    layout.getControlPanel()->setOnPlayPauseClicked([&stages]() {
        // The button toggles its own visual state (m_isPlaying, label,
        // accent) internally. Just tell the active stage to toggle pause.
        if (stages.current()) stages.current()->setPaused(!stages.current()->getPaused());
    });
    layout.getControlPanel()->setOnResetClicked([&stages]() {
        if (stages.current()) stages.current()->reset();
    });
    layout.getControlPanel()->setOnRadiusChanged([&stages](float v) {
        if (stages.current()) stages.current()->setParam("orbitRadius", v);
    });
    layout.getControlPanel()->setOnEccentricityChanged([&stages](float v) {
        if (stages.current()) stages.current()->setParam("eccentricity", v);
    });
    layout.getControlPanel()->setOnQuantumLevelChanged([&stages](float v) {
        if (stages.current()) stages.current()->setParam("level", v);
    });
    layout.getControlPanel()->setOnWaveCountChanged([&stages](float v) {
        if (stages.current()) stages.current()->setParam("waveCount", v);
    });
    layout.getControlPanel()->setOnPointDensityChanged([&stages](float v) {
        if (stages.current()) stages.current()->setParam("pointDensity", v);
    });
    layout.getControlPanel()->setOnPhotonJumpClicked([&stages]() {
        if (stages.current()) stages.current()->trigger("photonJump");
    });

    // The navigator's OnStageChange fires when the user clicks a stage node or
    // the Prev/Next buttons. The UIStageNavigator wires those buttons
    // internally; only this single callback is needed here.
    layout.getNavigator()->setOnStageChange([&stages, &layout](atomica::ui::StageId stage) {
        if (stages.current()) stages.current()->onExit();
        stages.switchTo(static_cast<std::size_t>(stage));
        if (stages.current()) stages.current()->onEnter();
        layout.setStage(stage);
    });

    // --- Render loop (v2: input callbacks + UI wiring + pause gating) ---
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;

        // Resolve the paused state once per frame: the active stage's
        // setPaused()/getPaused() chain is the source of truth for whether
        // update(dt) runs. The ControlPanel button toggled the stage's
        // paused flag directly, and the loop reads it back here.
        bool paused = false;
        if (stages.current()) paused = stages.current()->getPaused();

        if (!paused) {
            if (stages.current()) stages.current()->update(dt);
        }

        // 1. Clear the canvas, then draw the layout (top bar, sidebars,
        //    viewport border) on top.
        canvas.clear(atomica::ui::Color::Background());
        layout.render(canvas);

        // 2. Render the active stage inside the viewport (R1 integration:
        //    the stage draws into the shared canvas, clipped to the viewport).
        if (stages.current()) {
            stages.current()->render(canvas, layout.getViewportBounds());
        }

        // 3. Upload the canvas pixels to a GL texture and draw one full-screen
        //    quad through the persistent renderer.
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas.getWidth(), canvas.getHeight(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.getPixelData());
        glBindTexture(GL_TEXTURE_2D, 0);

        quad.render(tex);
        glDeleteTextures(1, &tex);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
