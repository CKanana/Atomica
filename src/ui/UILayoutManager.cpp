#include "UILayoutManager.h"

namespace atomica::ui {

UILayoutManager::UILayoutManager()
    : UILayoutManager(1280, 720) {}

UILayoutManager::UILayoutManager(int width, int height)
    : m_windowWidth(width), m_windowHeight(height) {

    m_stageNavigator = std::make_shared<UIStageNavigator>();
    m_infoPanel = std::make_shared<UIInfoPanel>();
    m_controlPanel = std::make_shared<UIControlPanel>();

    // Connect navigator to layout stage update
    m_stageNavigator->setOnStageChange([this](StageId stage) {
        setStage(stage);
    });

    updateLayout(m_windowWidth, m_windowHeight);
}

void UILayoutManager::updateLayout(int screenWidth, int screenHeight) {
    m_windowWidth = screenWidth;
    m_windowHeight = screenHeight;

    // 1. Top Bar Bounds (Full screen width)
    m_stageNavigator->updateBounds(Rect(0, 0, m_windowWidth, m_topBarHeight));

    // 2. Left Sidebar Bounds
    int mainAreaY = m_topBarHeight;
    int mainAreaHeight = m_windowHeight - m_topBarHeight;

    m_infoPanel->updateBounds(Rect(0, mainAreaY, m_leftSidebarWidth, mainAreaHeight));

    // 3. Right Sidebar Bounds
    int rightX = m_windowWidth - m_rightSidebarWidth;
    m_controlPanel->updateBounds(Rect(rightX, mainAreaY, m_rightSidebarWidth, mainAreaHeight));

    // 4. Central Viewport Bounds (Allocated for atomic model rendering)
    int viewportX = m_leftSidebarWidth;
    int viewportWidth = rightX - viewportX;
    m_viewportBounds = Rect(viewportX, mainAreaY, viewportWidth, mainAreaHeight);
}

void UILayoutManager::setStage(StageId stage) {
    if (m_activeStage != stage) {
        m_activeStage = stage;
        m_stageNavigator->setStage(m_activeStage);
        m_infoPanel->setStage(m_activeStage);
        m_controlPanel->setStage(m_activeStage);

        if (m_onStageChanged) {
            m_onStageChanged(m_activeStage);
        }
    }
}

void UILayoutManager::render(ICanvas& canvas) {
    // 1. Central Viewport Background & subtle grid / border
    canvas.fillRect(m_viewportBounds, Color::Background());
    canvas.drawRect(m_viewportBounds, Color::Border());

    // 2. Render Left and Right Panels
    m_infoPanel->render(canvas);
    m_controlPanel->render(canvas);

    // 3. Render Top Navigation Bar
    m_stageNavigator->render(canvas);
}

bool UILayoutManager::handleMouseMove(int mouseX, int mouseY) {
    if (m_stageNavigator->onMouseMove(mouseX, mouseY)) return true;
    if (m_infoPanel->onMouseMove(mouseX, mouseY)) return true;
    if (m_controlPanel->onMouseMove(mouseX, mouseY)) return true;
    return false;
}

bool UILayoutManager::handleMouseDown(int mouseX, int mouseY) {
    if (m_stageNavigator->onMouseDown(mouseX, mouseY)) return true;
    if (m_infoPanel->onMouseDown(mouseX, mouseY)) return true;
    if (m_controlPanel->onMouseDown(mouseX, mouseY)) return true;
    return false;
}

bool UILayoutManager::handleMouseUp(int mouseX, int mouseY) {
    bool handled = false;
    if (m_stageNavigator->onMouseUp(mouseX, mouseY)) handled = true;
    if (m_infoPanel->onMouseUp(mouseX, mouseY)) handled = true;
    if (m_controlPanel->onMouseUp(mouseX, mouseY)) handled = true;
    return handled;
}

} // namespace atomica::ui
