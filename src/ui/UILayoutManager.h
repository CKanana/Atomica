#pragma once

#include "Types.h"
#include "ICanvas.h"
#include "UIStageNavigator.h"
#include "UIInfoPanel.h"
#include "UIControlPanel.h"
#include <memory>
#include <functional>

namespace atomica::ui {

/**
 * Root Layout Manager & Coordinator
 * 
 * Manages responsive layout positioning across window dimensions:
 * - Top Bar: Stage Navigator (Progress & Prev/Next)
 * - Left Sidebar: Historical Model Inspector & Failure Analysis
 * - Right Sidebar: Stage-Specific Controls & Simulation Tweaks
 * - Central Viewport: The drawing region for the atomic model renderers
 */
class UILayoutManager {
public:
    using StageChangeCallback = std::function<void(StageId)>;
    // Callback invoked by render() after the viewport is cleared and bordered,
    // giving the host/stage a place to draw the atomic model without editing the
    // layout or the demo. Receives the canvas and the viewport rectangle.
    using ViewportRenderCallback = std::function<void(ICanvas&, const Rect&)>;

private:
    int m_windowWidth = 1280;
    int m_windowHeight = 720;

    int m_topBarHeight = 56;
    int m_leftSidebarWidth = 290;
    int m_rightSidebarWidth = 290;

    Rect m_viewportBounds;
    Color m_viewportBackground = Color::Background();
    ViewportRenderCallback m_onRenderViewport;

    std::shared_ptr<UIStageNavigator> m_stageNavigator;
    std::shared_ptr<UIInfoPanel> m_infoPanel;
    std::shared_ptr<UIControlPanel> m_controlPanel;

    StageId m_activeStage = StageId::PLANETARY;
    StageChangeCallback m_onStageChanged;

public:
    UILayoutManager();
    explicit UILayoutManager(int width, int height);

    void updateLayout(int screenWidth, int screenHeight);

    void setStage(StageId stage);
    StageId getActiveStage() const { return m_activeStage; }

    void setOnStageChanged(StageChangeCallback callback) { m_onStageChanged = std::move(callback); }

    // Viewport background colour (defaults to Color::Background())
    void setViewportBackground(const Color& color) { m_viewportBackground = color; }
    Color getViewportBackground() const { return m_viewportBackground; }

    // Register the viewport drawing routine (drawn after clear + border)
    void setOnRenderViewport(ViewportRenderCallback callback) { m_onRenderViewport = std::move(callback); }

    // Access to components
    std::shared_ptr<UIStageNavigator> getNavigator() const { return m_stageNavigator; }
    std::shared_ptr<UIInfoPanel> getInfoPanel() const { return m_infoPanel; }
    std::shared_ptr<UIControlPanel> getControlPanel() const { return m_controlPanel; }

    // Bounds for the central 3D/2D atomic model canvas
    const Rect& getViewportBounds() const { return m_viewportBounds; }

    // Event propagation
    void render(ICanvas& canvas);
    bool handleMouseMove(int mouseX, int mouseY);
    bool handleMouseDown(int mouseX, int mouseY);
    bool handleMouseUp(int mouseX, int mouseY);
};

} // namespace atomica::ui
