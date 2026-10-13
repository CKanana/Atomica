#pragma once

#include "ICanvas.h"
#include <vector>
#include <string>

namespace atomica::ui {

/**
 * Pure C++ Software Rasterizer Canvas
 * 
 * Satisfies the Checkpoint 1 requirement by implementing the core
 * computer graphics algorithms from scratch in pure C++:
 * - Bresenham's Line Algorithm
 * - Cohen-Sutherland Line Clipping
 * - Midpoint Circle Algorithm
 * - Midpoint Ellipse Algorithm
 * - Scanline Polygon / Box Fill
 * - Cubic Bézier Curve (Parametric evaluation)
 * - Alpha Blending
 * - 8x8 Bitmap Font Rasterization
 * - BMP & PPM file exporting (for offline verification and grading)
 */
class SoftwareCanvas : public ICanvas {
private:
    int m_width = 0;
    int m_height = 0;
    std::vector<Color> m_pixels;
    Rect m_clipRect;

    // Outcode bits for Cohen-Sutherland Line Clipping
    static constexpr int INSIDE = 0; // 0000
    static constexpr int LEFT   = 1; // 0001
    static constexpr int RIGHT  = 2; // 0010
    static constexpr int BOTTOM = 4; // 0100
    static constexpr int TOP    = 8; // 1000

    int computeOutCode(int x, int y, const Rect& clip) const;
    bool clipLine(int& x0, int& y0, int& x1, int& y1, const Rect& clip) const;

public:
    SoftwareCanvas() = default;
    SoftwareCanvas(int width, int height);

    void resize(int width, int height);

    int getWidth() const override { return m_width; }
    int getHeight() const override { return m_height; }

    void clear(const Color& color) override;
    void drawPixel(int x, int y, const Color& color) override;

    // --- Checkpoint 1 Algorithms ---

    // 1. Line Drawing (Bresenham)
    void drawLine(int x0, int y0, int x1, int y1, const Color& color) override;

    // 2. Rectangle & Polygon Scanline Fill
    void drawRect(const Rect& rect, const Color& color) override;
    void fillRect(const Rect& rect, const Color& color) override;

    // 3. Midpoint Circle Algorithm & Filled Circle
    void drawCircle(int cx, int cy, int radius, const Color& color) override;
    void fillCircle(int cx, int cy, int radius, const Color& color) override;

    // 4. Midpoint Ellipse Algorithm
    void drawEllipse(int cx, int cy, int rx, int ry, const Color& color) override;

    // 5. Cubic Bézier Curve
    void drawBezier(const Point2D& p0, const Point2D& p1,
                    const Point2D& p2, const Point2D& p3,
                    const Color& color) override;

    // 6. Text Rendering (8x8 Bitmap Font)
    void drawText(int x, int y, const std::string& text, const Color& color) override;

    // 7. Clipping
    void setClipRect(const Rect& clipRect) override;
    void resetClipRect() override;

    // --- Output & OpenGL Bridge ---
    const Color* getPixelData() const { return m_pixels.data(); }
    const std::vector<Color>& getPixels() const { return m_pixels; }

    // Export to BMP or PPM for easy visual verification without third-party tools
    bool saveToBMP(const std::string& filepath) const;
    bool saveToPPM(const std::string& filepath) const;
};

} // namespace atomica::ui
