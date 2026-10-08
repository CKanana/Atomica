#include "UIStageNavigator.h"

namespace atomica::ui {

UIStageNavigator::UIStageNavigator()
    : UIStageNavigator(Rect(0, 0, 1024, 60), nullptr) {}

UIStageNavigator::UIStageNavigator(const Rect& bounds, StageChangeCallback callback)
    : m_onStageChange(std::move(callback)) {
    m_bounds = bounds;

    // Initialize 5 Historical Stages
    m_nodes = {
        { StageId::PLANETARY,          "1. Planetary", "1911", {} },
        { StageId::BOHR,               "2. Bohr",      "1913", {} },
        { StageId::STANDING_WAVES,     "3. Waves",     "1924", {} },
        { StageId::PROBABILITY_CLOUDS, "4. Clouds",    "1926", {} },
        { StageId::SHADED_RENDERING,   "5. Shaded",    "1930", {} }
    };

    m_btnPrev = std::make_shared<UIButton>("< Prev", Rect(0, 0, 70, 32), [this]() {
        prevStage();
    });

    m_btnNext = std::make_shared<UIButton>("Next >", Rect(0, 0, 70, 32), [this]() {
        nextStage();
    });

    updateNodeLayout();
}

void UIStageNavigator::updateBounds(const Rect& bounds) {
    m_bounds = bounds;
    updateNodeLayout();
}

void UIStageNavigator::updateNodeLayout() {
    int btnWidth = 70;
    int btnHeight = 32;
    int paddingX = 16;
    int centerY = m_bounds.y + (m_bounds.height - btnHeight) / 2;

    // Previous Button (far left)
    m_btnPrev->setBounds(Rect(m_bounds.x + paddingX, centerY, btnWidth, btnHeight));

    // Next Button (far right)
    m_btnNext->setBounds(Rect(m_bounds.right() - paddingX - btnWidth, centerY, btnWidth, btnHeight));

    // Distribute Stage Nodes evenly between Prev and Next buttons
    int startX = m_bounds.x + paddingX + btnWidth + 24;
    int endX = m_bounds.right() - paddingX - btnWidth - 24;
    int availableWidth = endX - startX;
    int count = static_cast<int>(m_nodes.size());

    if (count > 0 && availableWidth > 0) {
        int nodeWidth = std::min(130, availableWidth / count);
        int spacing = (availableWidth - (nodeWidth * count)) / (count > 1 ? (count - 1) : 1);

        for (int i = 0; i < count; ++i) {
            int nx = startX + i * (nodeWidth + spacing);
            int ny = m_bounds.y + 10;
            m_nodes[i].bounds = Rect(nx, ny, nodeWidth, m_bounds.height - 20);
        }
    }
}

void UIStageNavigator::setStage(StageId stage) {
    if (m_currentStage != stage) {
        m_currentStage = stage;
        if (m_onStageChange) {
            m_onStageChange(m_currentStage);
        }
    }
}

void UIStageNavigator::nextStage() {
    int next = static_cast<int>(m_currentStage) + 1;
    if (next <= static_cast<int>(StageId::SHADED_RENDERING)) {
        setStage(static_cast<StageId>(next));
    }
}

void UIStageNavigator::prevStage() {
    int prev = static_cast<int>(m_currentStage) - 1;
    if (prev >= static_cast<int>(StageId::PLANETARY)) {
        setStage(static_cast<StageId>(prev));
    }
}

void UIStageNavigator::render(ICanvas& canvas) {
    if (!m_visible) return;

    // 1. Bar Background & Bottom Border
    canvas.fillRect(m_bounds, Color::PanelSurface());
    canvas.drawLine(m_bounds.x, m_bounds.bottom() - 1, m_bounds.right(), m_bounds.bottom() - 1, Color::Border());

    // 2. Connecting timeline line between stages
    if (m_nodes.size() >= 2) {
        int lineY = m_bounds.y + (m_bounds.height / 2);
        int startLineX = m_nodes.front().bounds.x + (m_nodes.front().bounds.width / 2);
        int endLineX = m_nodes.back().bounds.x + (m_nodes.back().bounds.width / 2);
        canvas.drawLine(startLineX, lineY, endLineX, lineY, Color::Border());
    }

    // 3. Render Stage Nodes
    for (const auto& node : m_nodes) {
        bool isActive = (node.id == m_currentStage);
        bool isPast = (static_cast<int>(node.id) < static_cast<int>(m_currentStage));

        // Node card background
        Color nodeBg = isActive ? Color(16, 42, 60) : (node.isHovered ? Color(28, 38, 56) : Color(20, 26, 38));
        canvas.fillRect(node.bounds, nodeBg);

        // Node border
        Color nodeBorder = isActive ? Color::BorderActive() : (isPast ? Color::TextHighlight() : Color::Border());
        canvas.drawRect(node.bounds, nodeBorder);

        // Small indicator circle on the card
        int circleX = node.bounds.x + 14;
        int circleY = node.bounds.y + (node.bounds.height / 2);
        int circleRadius = 5;

        Color circleColor = isActive ? Color::BorderActive() : (isPast ? Color::TextSecondary() : Color::Border());
        canvas.fillCircle(circleX, circleY, circleRadius, circleColor);

        // Stage Title
        Color textColor = isActive ? Color::TextPrimary() : Color::TextSecondary();
        canvas.drawText(node.bounds.x + 26, node.bounds.y + 8, node.label, textColor);

        // Stage Year
        Color yearColor = isActive ? Color::TextHighlight() : Color(100, 116, 139);
        canvas.drawText(node.bounds.x + 26, node.bounds.y + 24, node.year, yearColor);
    }

    // 4. Render Prev and Next Buttons
    m_btnPrev->setEnabled(static_cast<int>(m_currentStage) > static_cast<int>(StageId::PLANETARY));
    m_btnNext->setEnabled(static_cast<int>(m_currentStage) < static_cast<int>(StageId::SHADED_RENDERING));

    m_btnPrev->render(canvas);
    m_btnNext->render(canvas);
}

bool UIStageNavigator::onMouseMove(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;
    UIComponent::onMouseMove(mouseX, mouseY);

    m_btnPrev->onMouseMove(mouseX, mouseY);
    m_btnNext->onMouseMove(mouseX, mouseY);

    for (auto& node : m_nodes) {
        node.isHovered = node.bounds.contains(mouseX, mouseY);
    }
    return m_isHovered;
}

bool UIStageNavigator::onMouseDown(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;

    if (m_btnPrev->onMouseDown(mouseX, mouseY)) return true;
    if (m_btnNext->onMouseDown(mouseX, mouseY)) return true;

    for (const auto& node : m_nodes) {
        if (node.bounds.contains(mouseX, mouseY)) {
            setStage(node.id);
            return true;
        }
    }
    return m_bounds.contains(mouseX, mouseY);
}

bool UIStageNavigator::onMouseUp(int mouseX, int mouseY) {
    if (!m_visible || !m_enabled) return false;

    if (m_btnPrev->onMouseUp(mouseX, mouseY)) return true;
    if (m_btnNext->onMouseUp(mouseX, mouseY)) return true;

    return m_bounds.contains(mouseX, mouseY);
}

} // namespace atomica::ui
