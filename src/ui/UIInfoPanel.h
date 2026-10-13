#pragma once

#include "UIPanel.h"
#include "Types.h"
#include <vector>

namespace atomica::ui {

/**
 * Left Sidebar: Historical & Theoretical Model Inspector
 * 
 * Displays the physics context, the scientist who proposed it,
 * the theoretical breakthrough, and the fundamental crisis ("Why it failed")
 * that forced the progression to the next historical model.
 */
class UIInfoPanel : public UIPanel {
private:
    StageMetadata m_currentMetadata;

    // Helper to wrap text into multiple lines given a max pixel width
    std::vector<std::string> wrapText(const std::string& text, int maxCharsPerLine) const;

public:
    UIInfoPanel();
    explicit UIInfoPanel(const Rect& bounds);

    void setStage(StageId stage);
    void updateBounds(const Rect& bounds);

    void render(ICanvas& canvas) override;
};

} // namespace atomica::ui
