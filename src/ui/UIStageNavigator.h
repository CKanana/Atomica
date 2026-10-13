#pragma once

#include "UIComponent.h"
#include "UIButton.h"
#include <functional>
#include <vector>
#include <memory>

namespace atomica::ui {

/**
 * Top Navigation Bar for Chronological Atomic Model Evolution
 * Renders the 5 sequential stages with active progress indicator,
 * stage buttons, and Prev/Next controls.
 */
class UIStageNavigator : public UIComponent {
public:
    using StageChangeCallback = std::function<void(StageId)>;

private:
    StageId m_currentStage = StageId::PLANETARY;
    StageChangeCallback m_onStageChange;

    std::shared_ptr<UIButton> m_btnPrev;
    std::shared_ptr<UIButton> m_btnNext;

    struct StageNode {
        StageId id;
        std::string label;
        std::string year;
        Rect bounds;
        bool isHovered = false;
    };
    std::vector<StageNode> m_nodes;

    void updateNodeLayout();

public:
    UIStageNavigator();
    explicit UIStageNavigator(const Rect& bounds, StageChangeCallback callback = nullptr);

    void setStage(StageId stage);
    StageId getStage() const { return m_currentStage; }

    void nextStage();
    void prevStage();

    void setOnStageChange(StageChangeCallback callback) { m_onStageChange = std::move(callback); }
    void updateBounds(const Rect& bounds);

    void render(ICanvas& canvas) override;
    bool onMouseMove(int mouseX, int mouseY) override;
    bool onMouseDown(int mouseX, int mouseY) override;
    bool onMouseUp(int mouseX, int mouseY) override;
};

} // namespace atomica::ui
