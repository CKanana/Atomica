
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

    // 1. Draw navigation bar
    canvas.fillRect(m_bounds, Color::PanelSurface());

    canvas.drawLine(
        m_bounds.x,
        m_bounds.bottom() - 1,
        m_bounds.right(),
        m_bounds.bottom() - 1,
        Color::Border()
    );

    // 2. Draw timeline
    if (m_nodes.size() >= 2) {
        int lineY = m_bounds.y + (m_bounds.height / 2);
        int startLineX = m_nodes.front().bounds.x
                       + (m_nodes.front().bounds.width / 2);

        int endLineX = m_nodes.back().bounds.x
                     + (m_nodes.back().bounds.width / 2);

        // Full timeline
        canvas.drawLine(
            startLineX, lineY,
            endLineX, lineY,
            Color::Border()
        );

        // Completed timeline portion
        if (static_cast<int>(m_currentStage) >
            static_cast<int>(StageId::PLANETARY)) {

            // StageId starts at 1, but vector indexing starts at 0
            int activeIndex = static_cast<int>(m_currentStage) - 1;

            if (activeIndex >= 0 &&
                activeIndex < static_cast<int>(m_nodes.size())) {

                int activeLineX = m_nodes[activeIndex].bounds.x
                                + (m_nodes[activeIndex].bounds.width / 2);

                canvas.drawLine(
                    startLineX, lineY,
                    activeLineX, lineY,
                    Color::SuccessGreen()
                );
            }
        }
    }

    // 3. Render stage nodes
    for (const auto& node : m_nodes) {
        bool isActive = (node.id == m_currentStage);

        bool isPast = (
            static_cast<int>(node.id) <
            static_cast<int>(m_currentStage)
        );

        bool isHovered = node.isHovered;

        // Stage background
        Color nodeBg = Color::PanelSurface();

        if (isPast) {
            nodeBg = Color(16, 42, 38);
        }
        else if (isActive) {
            nodeBg = Color(16, 48, 64);
        }
        else if (isHovered) {
            nodeBg = Color::ControlHover();
        }

        canvas.fillRect(node.bounds, nodeBg);

        // Stage border
        Color nodeBorder = Color::Border();

        if (isPast) {
            nodeBorder = Color::SuccessGreen();
        }
        else if (isActive) {
            nodeBorder = Color::BorderActive();
        }

        canvas.drawRect(node.bounds, nodeBorder);

        // Stage indicator
        int circleX = node.bounds.x + 14;
        int circleY = node.bounds.y + (node.bounds.height / 2);
        int circleRadius = isActive ? 6 : 5;

        Color circleColor = Color::Border();

        if (isPast) {
            circleColor = Color::SuccessGreen();
        }
        else if (isActive) {
            circleColor = Color::ElectronCyan();
        }

        canvas.fillCircle(
            circleX,
            circleY,
            circleRadius,
            circleColor
        );

        // Stage title
        Color textColor = Color::MutedText();

        if (isPast) {
            textColor = Color::SuccessGreen();
        }
        else if (isActive) {
            textColor = Color::TextPrimary();
        }
        else if (isHovered) {
            textColor = Color::TextSecondary();
        }

        canvas.drawText(
            node.bounds.x + 26,
            node.bounds.y + 8,
            node.label,
            textColor
        );

        // Stage year
        Color yearColor = Color::MutedText();

        if (isPast) {
            yearColor = Color::SuccessGreen();
        }
        else if (isActive) {
            yearColor = Color::TextHighlight();
        }

        canvas.drawText(
            node.bounds.x + 26,
            node.bounds.y + 24,
            node.year,
            yearColor
        );
    }

    // 4. Render navigation buttons
    m_btnPrev->setEnabled(
        static_cast<int>(m_currentStage) >
        static_cast<int>(StageId::PLANETARY)
    );

    m_btnNext->setEnabled(
        static_cast<int>(m_currentStage) <
        static_cast<int>(StageId::SHADED_RENDERING)
    );

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
