#include "UIButton.h"

namespace atomica::ui {

UIButton::UIButton(const std::string& label, const Rect& bounds, ClickCallback onClick)
    : m_label(label), m_onClick(std::move(onClick)) {
    m_bounds = bounds;
}

void UIButton::setAccent(bool isAccent) {
    m_isAccent = isAccent;
    if (m_isAccent) {
        m_colorNormal  = Color(0, 115, 140);
        m_colorHover   = Color(0, 160, 195);
        m_colorPressed = Color(0, 90, 110);
    } else {
        m_colorNormal  = Color(28, 38, 56);
        m_colorHover   = Color(38, 52, 78);
        m_colorPressed = Color(16, 24, 38);
    }
}

void UIButton::render(ICanvas& canvas) {
    if (!m_visible) return;

    // Pick background color based on interaction state
    Color bg = m_colorNormal;
    if (!m_enabled) {
        bg = Color(20, 24, 32);
    } else if (m_isPressed) {
        bg = m_colorPressed;
    } else if (m_isHovered) {
        bg = m_colorHover;
    }

    // Draw background
    canvas.fillRect(m_bounds, bg);

    // Draw border
    Color border = (m_isHovered || m_isPressed) ? m_colorBorderActive : m_colorBorder;
    canvas.drawRect(m_bounds, border);

    // Draw centered / left text
    int textX = m_bounds.x + 10;
    int textY = m_bounds.y + (m_bounds.height / 2) - 5;
    Color textColor = m_enabled ? (m_isHovered ? Color::TextHighlight() : m_colorText) : Color::TextSecondary();
    canvas.drawText(textX, textY, m_label, textColor);
}

bool UIButton::onMouseUp(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;

    bool wasPressed = m_isPressed;
    m_isPressed = false;

    if (wasPressed && m_bounds.contains(mouseX, mouseY)) {
        if (m_onClick) {
            m_onClick();
        }
        return true;
    }
    return false;
}

} // namespace atomica::ui
