#pragma once

#include "UIComponent.h"
#include <vector>
#include <memory>
#include <string>

namespace atomica::ui {

/**
 * Container Panel / Card Component
 * Renders a dark frosted panel with borders, an optional header, and child elements.
 */
class UIPanel : public UIComponent {
protected:
    std::string m_title;
    Color m_backgroundColor = Color::PanelSurface();
    Color m_borderColor = Color::Border();
    Color m_headerColor = Color::PanelHeader();
    int m_headerHeight = 28;
    bool m_showHeader = true;

    std::vector<std::shared_ptr<UIComponent>> m_children;

public:
    UIPanel() = default;
    UIPanel(const std::string& title, const Rect& bounds, bool showHeader = true);

    void addChild(std::shared_ptr<UIComponent> child);
    void clearChildren();

    void setTitle(const std::string& title) { m_title = title; }
    const std::string& getTitle() const { return m_title; }

    void setBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void setBorderColor(const Color& color) { m_borderColor = color; }

    void render(ICanvas& canvas) override;
    bool onMouseMove(int mouseX, int mouseY) override;
    bool onMouseDown(int mouseX, int mouseY) override;
    bool onMouseUp(int mouseX, int mouseY) override;
};

} // namespace atomica::ui
