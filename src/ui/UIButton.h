#pragma once

#include "UIComponent.h"
#include <string>
#include <functional>

namespace atomica::ui {

/**
 * Interactive Clickable Button Component
 */
class UIButton : public UIComponent {
public:
    using ClickCallback = std::function<void()>;

private:
    std::string m_label;
    ClickCallback m_onClick;

    Color m_colorNormal   = Color(28, 38, 56);
    Color m_colorHover    = Color(38, 52, 78);
    Color m_colorPressed  = Color(16, 24, 38);
    Color m_colorBorder   = Color::Border();
    Color m_colorBorderActive = Color::BorderActive();
    Color m_colorText     = Color::TextPrimary();

    bool m_isAccent = false;

public:
    UIButton() = default;
    UIButton(const std::string& label, const Rect& bounds, ClickCallback onClick = nullptr);

    void setLabel(const std::string& label) { m_label = label; }
    const std::string& getLabel() const { return m_label; }

    void setOnClick(ClickCallback callback) { m_onClick = std::move(callback); }
    void setAccent(bool isAccent);

    void render(ICanvas& canvas) override;
    bool onMouseUp(int mouseX, int mouseY) override;
};

} // namespace atomica::ui
