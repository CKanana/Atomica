#include "UIControlPanel.h"

namespace atomica::ui {

UIControlPanel::UIControlPanel()
    : UIControlPanel(Rect(724, 60, 300, 700)) {}

UIControlPanel::UIControlPanel(const Rect& bounds)
    : UIPanel("STAGE CONTROLS", bounds, true) {
    
    // 1. Common Playback Controls
    m_sliderSpeed = std::make_shared<UISlider>("Speed", 0.1f, 3.0f, 1.0f, Rect(), nullptr, 1);
    
    m_btnPlayPause = std::make_shared<UIButton>("Pause ||", Rect(), [this]() {
        m_isPlaying = !m_isPlaying;
        m_btnPlayPause->setLabel(m_isPlaying ? "Pause ||" : "Play >");
        m_btnPlayPause->setAccent(!m_isPlaying);
    });

    m_btnReset = std::make_shared<UIButton>("Reset Model", Rect());

    // 2. Stage-Specific Controls
    m_sliderRadius = std::make_shared<UISlider>("Orbit Radius (r)", 50.0f, 250.0f, 120.0f, Rect(), nullptr, 0);
    m_sliderEccentricity = std::make_shared<UISlider>("Eccentricity (e)", 0.0f, 0.85f, 0.0f, Rect(), nullptr, 2);

    m_sliderQuantumLevel = std::make_shared<UISlider>("Energy Level (n)", 1.0f, 4.0f, 1.0f, Rect(), nullptr, 0);
    m_btnPhotonJump = std::make_shared<UIButton>("Trigger Photon Jump", Rect());
    m_btnPhotonJump->setAccent(true);

    m_sliderWaveCount = std::make_shared<UISlider>("Wave Count (n)", 2.0f, 8.0f, 3.0f, Rect(), nullptr, 0);
    m_sliderWaveAmplitude = std::make_shared<UISlider>("Wave Amplitude", 5.0f, 30.0f, 15.0f, Rect(), nullptr, 1);

    m_sliderPointDensity = std::make_shared<UISlider>("Point Density", 1000.0f, 50000.0f, 15000.0f, Rect(), nullptr, 0);
    m_sliderLightIntensity = std::make_shared<UISlider>("Light Intensity", 0.1f, 2.0f, 1.0f, Rect(), nullptr, 2);

    rebuildControlsForStage();
}

void UIControlPanel::setStage(StageId stage) {
    m_currentStage = stage;
    rebuildControlsForStage();
}

void UIControlPanel::updateBounds(const Rect& bounds) {
    m_bounds = bounds;
    rebuildControlsForStage();
}

void UIControlPanel::rebuildControlsForStage() {
    clearChildren();

    int padding = 16;
    int curY = m_bounds.y + m_headerHeight + padding;
    int ctrlWidth = m_bounds.width - (padding * 2);
    int sliderHeight = 38;
    int btnHeight = 32;

    // --- Playback Section ---
    m_sliderSpeed->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
    addChild(m_sliderSpeed);
    curY += sliderHeight + 12;

    int halfWidth = (ctrlWidth - 8) / 2;
    m_btnPlayPause->setBounds(Rect(m_bounds.x + padding, curY, halfWidth, btnHeight));
    addChild(m_btnPlayPause);

    m_btnReset->setBounds(Rect(m_bounds.x + padding + halfWidth + 8, curY, halfWidth, btnHeight));
    addChild(m_btnReset);
    curY += btnHeight + 20;

    // --- Stage-Specific Controls ---
    switch (m_currentStage) {
        case StageId::PLANETARY:
            m_sliderRadius->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderRadius);
            curY += sliderHeight + 12;

            m_sliderEccentricity->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderEccentricity);
            break;

        case StageId::BOHR:
            m_sliderQuantumLevel->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderQuantumLevel);
            curY += sliderHeight + 16;

            m_btnPhotonJump->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, btnHeight));
            addChild(m_btnPhotonJump);
            break;

        case StageId::STANDING_WAVES:
            m_sliderWaveCount->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderWaveCount);
            curY += sliderHeight + 12;

            m_sliderWaveAmplitude->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderWaveAmplitude);
            break;

        case StageId::PROBABILITY_CLOUDS:
            m_sliderPointDensity->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderPointDensity);
            break;

        case StageId::SHADED_RENDERING:
            m_sliderLightIntensity->setBounds(Rect(m_bounds.x + padding, curY, ctrlWidth, sliderHeight));
            addChild(m_sliderLightIntensity);
            break;
    }
}

void UIControlPanel::setOnSpeedChanged(FloatCallback cb) { m_sliderSpeed->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnRadiusChanged(FloatCallback cb) { m_sliderRadius->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnEccentricityChanged(FloatCallback cb) { m_sliderEccentricity->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnQuantumLevelChanged(FloatCallback cb) { m_sliderQuantumLevel->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnWaveCountChanged(FloatCallback cb) { m_sliderWaveCount->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnPointDensityChanged(FloatCallback cb) { m_sliderPointDensity->setOnValueChanged(std::move(cb)); }
void UIControlPanel::setOnPhotonJumpClicked(VoidCallback cb) { m_btnPhotonJump->setOnClick(std::move(cb)); }
void UIControlPanel::setOnPlayPauseClicked(VoidCallback cb) { m_btnPlayPause->setOnClick(std::move(cb)); }
void UIControlPanel::setOnResetClicked(VoidCallback cb) { m_btnReset->setOnClick(std::move(cb)); }

} // namespace atomica::ui
