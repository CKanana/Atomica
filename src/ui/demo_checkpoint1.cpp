#include "Types.h"
#include "SoftwareCanvas.h"
#include "UILayoutManager.h"
#include <iostream>
#include <cmath>

using namespace atomica::ui;

/**
 * Checkpoint 1 Demo - Pure C++ Technical Foundation
 * 
 * Demonstrates:
 * 1. Pure C++ UI Layout Manager (Top Bar, Left Model Inspector, Right Controls)
 * 2. Graphics Algorithms applied to Atomica components:
 *    - Midpoint Circle Algorithm: Nucleus, electron nodes, slider knobs
 *    - Midpoint Ellipse Algorithm: Planetary elliptical orbits (e > 0)
 *    - Bresenham Line Algorithm: UI borders, timeline connections, coordinate crosshairs
 *    - Scanline Polygon / Box Fill: UI card surfaces, active state fills
 *    - Cubic Bézier Curves: de Broglie wave preview & photon transition trajectory
 *    - 8x8 Bitmap Font Rasterization: UI labels, scientific parameters, failure callout
 */
int main() {
    constexpr int WIDTH = 1280;
    constexpr int HEIGHT = 720;

    std::cout << "====================================================\n";
    std::cout << " Atomica - Checkpoint 1: Technical Foundation\n";
    std::cout << " Pure C++ UI Layout & Graphics Algorithms Rasterizer\n";
    std::cout << "====================================================\n\n";

    // 1. Create Pure C++ Software Canvas
    SoftwareCanvas canvas(WIDTH, HEIGHT);
    canvas.clear(Color::Background());

    // 2. Initialize UI Layout Manager
    UILayoutManager layout(WIDTH, HEIGHT);
    layout.setStage(StageId::PLANETARY);

    // 3. Render the UI Framework (Top Bar, Left Inspector, Right Controls)
    layout.render(canvas);

    // 4. Render Atomic Model Preview in Central Viewport using Checkpoint 1 Algorithms
    Rect vp = layout.getViewportBounds();
    int centerX = vp.x + (vp.width / 2);
    int centerY = vp.y + (vp.height / 2);

    // Set clipping so atomic model drawing never spills outside viewport
    canvas.setClipRect(vp);

    // A. Subdued Coordinate Crosshairs (Bresenham Line)
    Color gridColor(24, 32, 48);
    canvas.drawLine(centerX - 240, centerY, centerX + 240, centerY, gridColor);
    canvas.drawLine(centerX, centerY - 240, centerX, centerY + 240, gridColor);

    // B. Circular Planetary Orbit (Midpoint Circle Algorithm)
    int r1 = 140;
    canvas.drawCircle(centerX, centerY, r1, Color(30, 60, 90));

    // C. Elliptical Planetary Orbit (Midpoint Ellipse Algorithm)
    int rx = 210;
    int ry = 120;
    canvas.drawEllipse(centerX, centerY, rx, ry, Color(40, 80, 115));

    // D. de Broglie Matter Wave Preview (Cubic Bézier Splines)
    // Four Bézier segments approximating oscillating wave lobes around the orbit
    Point2D w0(centerX - 140, centerY);
    Point2D c1(centerX - 140, centerY - 60);
    Point2D c2(centerX - 60,  centerY - 170);
    Point2D w1(centerX,       centerY - 140);
    canvas.drawBezier(w0, c1, c2, w1, Color(0, 180, 216));

    Point2D w2(centerX + 140, centerY);
    Point2D c3(centerX + 60,  centerY - 110);
    Point2D c4(centerX + 140, centerY - 60);
    canvas.drawBezier(w1, c3, c4, w2, Color(0, 180, 216));

    // E. Central Nucleus (Midpoint Circle Fill)
    int nucleusRadius = 14;
    canvas.fillCircle(centerX, centerY, nucleusRadius, Color::NucleusAmber());
    canvas.drawCircle(centerX, centerY, nucleusRadius + 3, Color(251, 191, 36, 120)); // Soft halo

    // F. Electron Nodes (Midpoint Circle Fill with Quantum Cyan)
    // Electron on circular orbit (angle ~ 45 deg)
    int e1X = centerX + static_cast<int>(r1 * 0.707f);
    int e1Y = centerY - static_cast<int>(r1 * 0.707f);
    canvas.fillCircle(e1X, e1Y, 7, Color::ElectronCyan());
    canvas.drawCircle(e1X, e1Y, 10, Color(0, 229, 255, 100)); // Glow ring

    // Electron on elliptical orbit
    int e2X = centerX - rx + 30;
    int e2Y = centerY + static_cast<int>(ry * 0.75f);
    canvas.fillCircle(e2X, e2Y, 7, Color::ElectronCyan());

    // G. Viewport HUD Annotations (8x8 Bitmap Font)
    canvas.drawText(vp.x + 20, vp.y + 20, "HYDROGEN ATOM (H-1) : STAGE 1 PREVIEW", Color::TextHighlight());
    canvas.drawText(vp.x + 20, vp.y + 36, "Orbits: Circle r=140px | Ellipse rx=210px, ry=120px", Color::TextSecondary());
    canvas.drawText(vp.x + 20, vp.y + 52, "Demonstrating: Bresenham, Midpoint Circle/Ellipse, Bezier", Color(100, 116, 139));

    // Telemetry badge
    Rect telemetryBox(vp.right() - 200, vp.bottom() - 65, 180, 50);
    canvas.fillRect(telemetryBox, Color(16, 22, 34));
    canvas.drawRect(telemetryBox, Color::Border());
    canvas.drawText(telemetryBox.x + 10, telemetryBox.y + 10, "Z = 1 (Proton: +1e)", Color::NucleusAmber());
    canvas.drawText(telemetryBox.x + 10, telemetryBox.y + 28, "e- = 1 (Charge: -1e)", Color::ElectronCyan());

    // Reset clipping
    canvas.resetClipRect();

    // 5. Save output to BMP image file
    const std::string bmpFile = "checkpoint1_ui_demo.bmp";
    std::cout << "Rendering raster image (" << WIDTH << "x" << HEIGHT << ")...\n";
    if (canvas.saveToBMP(bmpFile)) {
        std::cout << "[SUCCESS] Saved high-resolution demo render to: " << bmpFile << "\n";
    } else {
        std::cerr << "[ERROR] Failed to save BMP image.\n";
        return 1;
    }

    std::cout << "\nAll Checkpoint 1 graphics algorithms executed in Pure C++ successfully!\n";
    return 0;
}
