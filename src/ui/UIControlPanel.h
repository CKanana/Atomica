#pragma once

#include "UIPanel.h"
#include "UIButton.h"
#include "UISlider.h"
#include "Types.h"
#include <vector>
#include <memory>
#include <functional>

namespace atomica::ui {

/**
 * Right Sidebar: Stage Controls & Interactive Physical Parameters
 * Exposes interactive sliders and buttons specific to the active atomic model.
 */
class UIControlPanel : public UIPanel {
public:
    using VoidCallback = std::function<void()>;
    using FloatCallback = std::function<void(float)>;

private:
    StageId m_currentStage = StageId::PLANETARY;

    // Common Controls
    std::shared_ptr<UISlider> m_sliderSpeed;
    std::shared_ptr<UIButton> m_btnPlayPause;
    std::shared_ptr<UIButton> m_btnReset;
    bool m_isPlaying = true;

    // Stage 1 (Planetary) Controls
    std::shared_ptr<UISlider> m_sliderRadius;
    std::shared_ptr<UISlider> m_sliderEccentricity;

    // Stage 2 (Bohr) Controls
    std::shared_ptr<UISlider> m_sliderQuantumLevel;
    std::shared_ptr<UIButton> m_btnPhotonJump;

    // Stage 3 (Waves) Controls
    std::shared_ptr<UISlider> m_sliderWaveCount;
    std::shared_ptr<UISlider> m_sliderWaveAmplitude;

    // Stage 4 & 5 (Clouds & Shaded) Controls
    std::shared_ptr<UISlider> m_sliderPointDensity;
    std::shared_ptr<UISlider> m_sliderLightIntensity;

    void rebuildControlsForStage();

public:
    UIControlPanel();
    explicit UIControlPanel(const Rect& bounds);

    void setStage(StageId stage);
    void updateBounds(const Rect& bounds);

    // Callbacks to notify the engine/stage of user changes
    void setOnSpeedChanged(FloatCallback cb);
    void setOnRadiusChanged(FloatCallback cb);
    void setOnEccentricityChanged(FloatCallback cb);
    void setOnQuantumLevelChanged(FloatCallback cb);
    void setOnWaveCountChanged(FloatCallback cb);
    void setOnPointDensityChanged(FloatCallback cb);
    void setOnPhotonJumpClicked(VoidCallback cb);
    void setOnPlayPauseClicked(VoidCallback cb);
    void setOnResetClicked(VoidCallback cb);
};

} // namespace atomica::ui
