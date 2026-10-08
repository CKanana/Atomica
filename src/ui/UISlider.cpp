#include "UISlider.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace atomica::ui {

UISlider::UISlider(const std::string& label, float minVal, float maxVal, float initialVal,
                   const Rect& bounds, ValueChangeCallback onChange, int decimals)
    : m_label(label), m_minValue(minVal), m_maxValue(maxVal),
      m_value(std::clamp(initialVal, minVal, maxVal)),
      m_decimalPlaces(decimals), m_onValueChanged(std::move(onChange)) {
    m_bounds = bounds;
}

void UISlider::setValue(float value) {
    float clamped = std::clamp(value, m_minValue, m_maxValue);
    if (clamped != m_value) {
        m_value = clamped;
        if (m_onValueChanged) {
            m_onValueChanged(m_value);
        }
    }
}

void UISlider::setRange(float minVal, float maxVal) {
    m_minValue = minVal;
    m_maxValue = maxVal;
    setValue(m_value);
}

Rect UISlider::getTrackRect() const {
    // Track occupies the bottom half of the slider bounds
    int trackY = m_bounds.y + 24;
    int trackHeight = 4;
    return Rect(m_bounds.x + 8, trackY, m_bounds.width - 16, trackHeight);
}

Point2D UISlider::getKnobCenter() const {
    Rect track = getTrackRect();
    float normalized = (m_maxValue > m_minValue) 
                     ? ((m_value - m_minValue) / (m_maxValue - m_minValue))
                     : 0.0f;
    int knobX = track.x + static_cast<int>(normalized * track.width);
    int knobY = track.y + (track.height / 2);
    return Point2D(knobX, knobY);
}

void UISlider::updateValueFromMouse(int mouseX) {
    Rect track = getTrackRect();
    if (track.width <= 0) return;

    float normalized = static_cast<float>(mouseX - track.x) / static_cast<float>(track.width);
    normalized = std::clamp(normalized, 0.0f, 1.0f);

    float newVal = m_minValue + normalized * (m_maxValue - m_minValue);
    setValue(newVal);
}

void UISlider::render(ICanvas& canvas) {
    if (!m_visible) return;

    // 1. Draw Label and Current Value text
    std::ostringstream ss;
    ss << m_label << ": " << std::fixed << std::setprecision(m_decimalPlaces) << m_value;
    canvas.drawText(m_bounds.x + 4, m_bounds.y + 4, ss.str(), Color::TextSecondary());

    // 2. Draw Track
    Rect track = getTrackRect();
    canvas.fillRect(track, Color(30, 41, 59));
    canvas.drawRect(track, Color::Border());

    // 3. Draw Active Fill from min to knob
    Point2D knob = getKnobCenter();
    int activeWidth = knob.x - track.x;
    if (activeWidth > 0) {
        Rect activeTrack(track.x, track.y, activeWidth, track.height);
        canvas.fillRect(activeTrack, Color::ElectronCyan());
    }

    // 4. Draw Knob (Circle algorithm)
    int knobRadius = 6;
    Color knobColor = m_isDragging ? Color::BorderActive() : Color::TextPrimary();
    canvas.fillCircle(knob.x, knob.y, knobRadius, knobColor);
    canvas.drawCircle(knob.x, knob.y, knobRadius, Color::Border());
}

bool UISlider::onMouseMove(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;
    UIComponent::onMouseMove(mouseX, mouseY);

    if (m_isDragging) {
        updateValueFromMouse(mouseX);
        return true;
    }
    return m_isHovered;
}

bool UISlider::onMouseDown(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;

    if (m_bounds.contains(mouseX, mouseY)) {
        m_isDragging = true;
        m_isPressed = true;
        updateValueFromMouse(mouseX);
        return true;
    }
    return false;
}

bool UISlider::onMouseUp(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;
    m_isDragging = false;
    m_isPressed = false;
    return m_bounds.contains(mouseX, mouseY);
}

} // namespace atomica::ui
