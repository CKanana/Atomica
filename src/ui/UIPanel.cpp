#include "UIPanel.h"

namespace atomica::ui {

UIPanel::UIPanel(const std::string& title, const Rect& bounds, bool showHeader)
    : m_title(title), m_showHeader(showHeader) {
    m_bounds = bounds;
}

void UIPanel::addChild(std::shared_ptr<UIComponent> child) {
    if (child) {
        m_children.push_back(child);
    }
}

void UIPanel::clearChildren() {
    m_children.clear();
}

void UIPanel::render(ICanvas& canvas) {
    if (!m_visible) return;

    // 1. Draw solid background
    canvas.fillRect(m_bounds, m_backgroundColor);

    // 2. Draw panel header bar if enabled
    if (m_showHeader && !m_title.empty()) {
        Rect headerRect(m_bounds.x, m_bounds.y, m_bounds.width, m_headerHeight);
        canvas.fillRect(headerRect, m_headerColor);
        canvas.drawRect(headerRect, m_borderColor);

        // Header title text
        canvas.drawText(m_bounds.x + 12, m_bounds.y + 7, m_title, Color::TextPrimary());
    }

    // 3. Draw outer border
    canvas.drawRect(m_bounds, m_borderColor);

    // 4. Render children
    for (auto& child : m_children) {
        if (child && child->isVisible()) {
            child->render(canvas);
        }
    }
}

bool UIPanel::onMouseMove(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;
    UIComponent::onMouseMove(mouseX, mouseY);

    bool handled = false;
    for (auto& child : m_children) {
        if (child && child->onMouseMove(mouseX, mouseY)) {
            handled = true;
        }
    }
    return handled || m_isHovered;
}

bool UIPanel::onMouseDown(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;
    if (!m_bounds.contains(mouseX, mouseY)) return false;

    for (auto& child : m_children) {
        if (child && child->onMouseDown(mouseX, mouseY)) {
            return true;
        }
    }
    m_isPressed = true;
    return true;
}

bool UIPanel::onMouseUp(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;

    bool handled = false;
    for (auto& child : m_children) {
        if (child && child->onMouseUp(mouseX, mouseY)) {
            handled = true;
        }
    }
    m_isPressed = false;
    return handled;
}

} // namespace atomica::ui
