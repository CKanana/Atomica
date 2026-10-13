#pragma once

#include "Types.h"
#include <string>

namespace atomica::ui {

/**
 * Abstract Canvas Interface for 2D Drawing Primitives
 * 
 * Enables UI components to be completely decoupled from the underlying
 * rendering technology (Pure C++ software framebuffer for Checkpoint 1,
 * or OpenGL textures/shaders for subsequent checkpoints).
 */
class ICanvas {
public:
    virtual ~ICanvas() = default;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual void clear(const Color& color) = 0;
    virtual void drawPixel(int x, int y, const Color& color) = 0;

    // Line drawing (Bresenham / DDA)
    virtual void drawLine(int x0, int y0, int x1, int y1, const Color& color) = 0;

    // Rectangle drawing & polygon filling
    virtual void drawRect(const Rect& rect, const Color& color) = 0;
    virtual void fillRect(const Rect& rect, const Color& color) = 0;

    // Circle drawing & filling (Midpoint circle algorithm)
    virtual void drawCircle(int cx, int cy, int radius, const Color& color) = 0;
    virtual void fillCircle(int cx, int cy, int radius, const Color& color) = 0;

    // Ellipse drawing (Midpoint ellipse algorithm)
    virtual void drawEllipse(int cx, int cy, int rx, int ry, const Color& color) = 0;

    // Parametric curves (Cubic Bézier curve)
    virtual void drawBezier(const Point2D& p0, const Point2D& p1, 
                            const Point2D& p2, const Point2D& p3, 
                            const Color& color) = 0;

    // Text rendering
    virtual void drawText(int x, int y, const std::string& text, const Color& color) = 0;

    // Clipping support (Cohen-Sutherland / Bounding box)
    virtual void setClipRect(const Rect& clipRect) = 0;
    virtual void resetClipRect() = 0;
};

} // namespace atomica::ui
