#pragma once

#include "UIComponent.h"
#include <string>
#include <functional>

namespace atomica::ui {

/**
 * Interactive Horizontal Slider Component
 * Supports real-time parameter tweaking with a label, track, knob, and value readout.
 */
class UISlider : public UIComponent {
public:
    using ValueChangeCallback = std::function<void(float)>;

private:
    std::string m_label;
    float m_minValue = 0.0f;
    float m_maxValue = 1.0f;
    float m_value = 0.5f;
    int m_decimalPlaces = 2;

    bool m_isDragging = false;
    ValueChangeCallback m_onValueChanged;

    Rect getTrackRect() const;
    Point2D getKnobCenter() const;
    void updateValueFromMouse(int mouseX);

public:
    UISlider() = default;
    UISlider(const std::string& label, float minVal, float maxVal, float initialVal,
             const Rect& bounds, ValueChangeCallback onChange = nullptr, int decimals = 2);

    void setValue(float value);
    float getValue() const { return m_value; }

    void setRange(float minVal, float maxVal);
    void setOnValueChanged(ValueChangeCallback callback) { m_onValueChanged = std::move(callback); }

    void render(ICanvas& canvas) override;
    bool onMouseMove(int mouseX, int mouseY) override;
    bool onMouseDown(int mouseX, int mouseY) override;
    bool onMouseUp(int mouseX, int mouseY) override;
};

} // namespace atomica::ui
