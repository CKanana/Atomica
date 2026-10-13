#pragma once

#include "Types.h"
#include "ICanvas.h"

namespace atomica::ui {

/**
 * Base Abstract Class for all UI Components
 */
class UIComponent {
protected:
    Rect m_bounds;
    bool m_visible = true;
    bool m_enabled = true;
    bool m_isHovered = false;
    bool m_isPressed = false;

public:
    virtual ~UIComponent() = default;

    virtual void render(ICanvas& canvas) = 0;

    virtual bool onMouseMove(int mouseX, int mouseY) {
        if (!m_visible || !m_enabled) return false;
        m_isHovered = m_bounds.contains(mouseX, mouseY);
        return m_isHovered;
    }

    virtual bool onMouseDown(int mouseX, int mouseY) {
        if (!m_visible || !m_enabled) return false;
        if (m_bounds.contains(mouseX, mouseY)) {
            m_isPressed = true;
            return true;
        }
        return false;
    }

    virtual bool onMouseUp(int mouseX, int mouseY) {
        if (!m_visible || !m_enabled) return false;
        bool wasPressed = m_isPressed;
        m_isPressed = false;
        return wasPressed && m_bounds.contains(mouseX, mouseY);
    }

    void setBounds(const Rect& bounds) { m_bounds = bounds; }
    const Rect& getBounds() const { return m_bounds; }

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    bool isHovered() const { return m_isHovered; }
    bool isPressed() const { return m_isPressed; }
};

} // namespace atomica::ui
