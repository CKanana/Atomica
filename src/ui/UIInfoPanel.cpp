#include "UIInfoPanel.h"
#include <sstream>

namespace atomica::ui {

UIInfoPanel::UIInfoPanel()
    : UIInfoPanel(Rect(0, 60, 300, 700)) {}

UIInfoPanel::UIInfoPanel(const Rect& bounds)
    : UIPanel("MODEL INSPECTOR", bounds, true) {
    setStage(StageId::PLANETARY);
}

void UIInfoPanel::setStage(StageId stage) {
    m_currentMetadata = getStageMetadata(stage);
}

void UIInfoPanel::updateBounds(const Rect& bounds) {
    m_bounds = bounds;
}

std::vector<std::string> UIInfoPanel::wrapText(const std::string& text, int maxCharsPerLine) const {
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string currentLine;

    while (words >> word) {
        if (currentLine.empty()) {
            currentLine = word;
        } else if (static_cast<int>(currentLine.length() + 1 + word.length()) <= maxCharsPerLine) {
            currentLine += " " + word;
        } else {
            lines.push_back(currentLine);
            currentLine = word;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

void UIInfoPanel::render(ICanvas& canvas) {
    if (!m_visible) return;

    // 1. Draw base panel
    UIPanel::render(canvas);

    int padding = 16;
    int curY = m_bounds.y + m_headerHeight + padding;
    int contentWidth = m_bounds.width - (padding * 2);
    int maxChars = std::max(20, contentWidth / 8);

    // --- Section 1: Stage Title & Proposer ---
    canvas.drawText(m_bounds.x + padding, curY, m_currentMetadata.title, Color::BorderActive());
    curY += 20;

    std::string proposedStr = "Proposed by: " + m_currentMetadata.scientist + " (" + std::to_string(m_currentMetadata.year) + ")";
    canvas.drawText(m_bounds.x + padding, curY, proposedStr, Color::TextSecondary());
    curY += 28;

    // Divider line
    canvas.drawLine(m_bounds.x + padding, curY, m_bounds.right() - padding, curY, Color::Border());
    curY += 16;

    // --- Section 2: Theoretical Hypothesis ---
    canvas.drawText(m_bounds.x + padding, curY, "[ Theoretical Basis ]", Color::TextPrimary());
    curY += 20;

    auto hypothesisLines = wrapText(m_currentMetadata.coreConcept, maxChars);
    for (const auto& line : hypothesisLines) {
        canvas.drawText(m_bounds.x + padding, curY, line, Color::TextSecondary());
        curY += 16;
    }
    curY += 16;

    // --- Section 3: Why It Failed / Physical Crisis ---
    Rect alertBox(m_bounds.x + padding, curY, contentWidth, 110);
    canvas.fillRect(alertBox, Color(40, 20, 24));
    canvas.drawRect(alertBox, Color::DangerRed());

    canvas.drawText(alertBox.x + 10, alertBox.y + 10, "! Why It Failed (Crisis):", Color::DangerRed());
    int alertY = alertBox.y + 30;

    auto failureLines = wrapText(m_currentMetadata.whyItFailed, maxChars - 2);
    for (const auto& line : failureLines) {
        canvas.drawText(alertBox.x + 10, alertY, line, Color(254, 202, 202));
        alertY += 15;
    }
    curY += alertBox.height + 20;

    // --- Section 4: Graphics Techniques Mapped ---
    canvas.drawText(m_bounds.x + padding, curY, "[ Graphics Techniques ]", Color::NucleusAmber());
    curY += 20;

    auto techLines = wrapText(m_currentMetadata.graphicsTechniques, maxChars);
    for (const auto& line : techLines) {
        canvas.drawText(m_bounds.x + padding, curY, line, Color::TextSecondary());
        curY += 16;
    }
}

} // namespace atomica::ui
