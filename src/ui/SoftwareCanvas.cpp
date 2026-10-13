#include "SoftwareCanvas.h"
#include "Font8x8.h"
#include <fstream>
#include <cmath>
#include <algorithm>

namespace atomica::ui {

SoftwareCanvas::SoftwareCanvas(int width, int height) {
    resize(width, height);
}

void SoftwareCanvas::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_pixels.assign(m_width * m_height, Color::Background());
    m_clipRect = Rect(0, 0, m_width, m_height);
}

void SoftwareCanvas::clear(const Color& color) {
    std::fill(m_pixels.begin(), m_pixels.end(), color);
}

void SoftwareCanvas::setClipRect(const Rect& clipRect) {
    int x0 = std::clamp(clipRect.x, 0, m_width);
    int y0 = std::clamp(clipRect.y, 0, m_height);
    int x1 = std::clamp(clipRect.right(), 0, m_width);
    int y1 = std::clamp(clipRect.bottom(), 0, m_height);
    m_clipRect = Rect(x0, y0, std::max(0, x1 - x0), std::max(0, y1 - y0));
}

void SoftwareCanvas::resetClipRect() {
    m_clipRect = Rect(0, 0, m_width, m_height);
}

void SoftwareCanvas::drawPixel(int x, int y, const Color& color) {
    if (!m_clipRect.contains(x, y)) return;
    if (color.a == 0) return;

    size_t index = static_cast<size_t>(y * m_width + x);
    if (color.a == 255) {
        m_pixels[index] = color;
    } else {
        // Alpha Blending: Out = Src * alpha + Dst * (1 - alpha)
        Color& dst = m_pixels[index];
        float alpha = static_cast<float>(color.a) / 255.0f;
        float invAlpha = 1.0f - alpha;

        dst.r = static_cast<uint8_t>(color.r * alpha + dst.r * invAlpha);
        dst.g = static_cast<uint8_t>(color.g * alpha + dst.g * invAlpha);
        dst.b = static_cast<uint8_t>(color.b * alpha + dst.b * invAlpha);
        dst.a = 255;
    }
}

// --- Cohen-Sutherland Line Clipping ---

int SoftwareCanvas::computeOutCode(int x, int y, const Rect& clip) const {
    int code = INSIDE;
    if (x < clip.x)             code |= LEFT;
    else if (x >= clip.right())  code |= RIGHT;
    if (y < clip.y)             code |= BOTTOM;
    else if (y >= clip.bottom()) code |= TOP;
    return code;
}

bool SoftwareCanvas::clipLine(int& x0, int& y0, int& x1, int& y1, const Rect& clip) const {
    int code0 = computeOutCode(x0, y0, clip);
    int code1 = computeOutCode(x1, y1, clip);

    while (true) {
        if ((code0 | code1) == 0) {
            // Both points inside
            return true;
        } else if (code0 & code1) {
            // Both points share outside zone -> trivially reject
            return false;
        } else {
            // Clip segment
            int codeOut = code0 ? code0 : code1;
            int x = 0;
            int y = 0;

            if (codeOut & TOP) {
                x = x0 + static_cast<int>((x1 - x0) * (clip.bottom() - 1 - y0) / (y1 - y0));
                y = clip.bottom() - 1;
            } else if (codeOut & BOTTOM) {
                x = x0 + static_cast<int>((x1 - x0) * (clip.y - y0) / (y1 - y0));
                y = clip.y;
            } else if (codeOut & RIGHT) {
                y = y0 + static_cast<int>((y1 - y0) * (clip.right() - 1 - x0) / (x1 - x0));
                x = clip.right() - 1;
            } else if (codeOut & LEFT) {
                y = y0 + static_cast<int>((y1 - y0) * (clip.x - x0) / (x1 - x0));
                x = clip.x;
            }

            if (codeOut == code0) {
                x0 = x;
                y0 = y;
                code0 = computeOutCode(x0, y0, clip);
            } else {
                x1 = x;
                y1 = y;
                code1 = computeOutCode(x1, y1, clip);
            }
        }
    }
}

// --- 1. Line Drawing: Bresenham's Line Algorithm ---

void SoftwareCanvas::drawLine(int x0, int y0, int x1, int y1, const Color& color) {
    int cx0 = x0;
    int cy0 = y0;
    int cx1 = x1;
    int cy1 = y1;

    // Clip against active scissor/clip rect before drawing
    if (!clipLine(cx0, cy0, cx1, cy1, m_clipRect)) return;

    int dx = std::abs(cx1 - cx0);
    int dy = -std::abs(cy1 - cy0);
    int sx = cx0 < cx1 ? 1 : -1;
    int sy = cy0 < cy1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        drawPixel(cx0, cy0, color);
        if (cx0 == cx1 && cy0 == cy1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            cx0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            cy0 += sy;
        }
    }
}

// --- 2. Rectangle & Polygon Scanline Fill ---

void SoftwareCanvas::drawRect(const Rect& rect, const Color& color) {
    if (rect.width <= 0 || rect.height <= 0) return;
    drawLine(rect.x, rect.y, rect.right() - 1, rect.y, color);
    drawLine(rect.right() - 1, rect.y, rect.right() - 1, rect.bottom() - 1, color);
    drawLine(rect.right() - 1, rect.bottom() - 1, rect.x, rect.bottom() - 1, color);
    drawLine(rect.x, rect.bottom() - 1, rect.x, rect.y, color);
}

void SoftwareCanvas::fillRect(const Rect& rect, const Color& color) {
    int startY = std::max(rect.y, m_clipRect.y);
    int endY = std::min(rect.bottom(), m_clipRect.bottom());
    int startX = std::max(rect.x, m_clipRect.x);
    int endX = std::min(rect.right(), m_clipRect.right());

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            drawPixel(x, y, color);
        }
    }
}

// --- 3. Midpoint Circle Algorithm ---

void SoftwareCanvas::drawCircle(int cx, int cy, int radius, const Color& color) {
    if (radius <= 0) {
        drawPixel(cx, cy, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius; // Midpoint decision parameter

    auto plot8 = [this, cx, cy, &color](int px, int py) {
        drawPixel(cx + px, cy + py, color);
        drawPixel(cx - px, cy + py, color);
        drawPixel(cx + px, cy - py, color);
        drawPixel(cx - px, cy - py, color);
        drawPixel(cx + py, cy + px, color);
        drawPixel(cx - py, cy + px, color);
        drawPixel(cx + py, cy - px, color);
        drawPixel(cx - py, cy - px, color);
    };

    plot8(x, y);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot8(x, y);
    }
}

void SoftwareCanvas::fillCircle(int cx, int cy, int radius, const Color& color) {
    if (radius <= 0) {
        drawPixel(cx, cy, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    auto drawScanline = [this, cx, cy, &color](int sx, int sy) {
        for (int px = cx - sx; px <= cx + sx; ++px) {
            drawPixel(px, cy + sy, color);
            drawPixel(px, cy - sy, color);
        }
    };

    while (x <= y) {
        drawScanline(x, y);
        drawScanline(y, x);

        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}

// --- 4. Midpoint Ellipse Algorithm ---

void SoftwareCanvas::drawEllipse(int cx, int cy, int rx, int ry, const Color& color) {
    if (rx <= 0 || ry <= 0) return;

    long long rx2 = static_cast<long long>(rx) * rx;
    long long ry2 = static_cast<long long>(ry) * ry;
    long long twoRx2 = 2 * rx2;
    long long twoRy2 = 2 * ry2;

    int x = 0;
    int y = ry;
    long long px = 0;
    long long py = twoRx2 * y;

    auto plot4 = [this, cx, cy, &color](int ex, int ey) {
        drawPixel(cx + ex, cy + ey, color);
        drawPixel(cx - ex, cy + ey, color);
        drawPixel(cx + ex, cy - ey, color);
        drawPixel(cx - ex, cy - ey, color);
    };

    plot4(x, y);

    // Region 1 (slope > -1)
    double p1 = ry2 - (rx2 * ry) + (0.25 * rx2);
    while (px < py) {
        x++;
        px += twoRy2;
        if (p1 < 0) {
            p1 += ry2 + px;
        } else {
            y--;
            py -= twoRx2;
            p1 += ry2 + px - py;
        }
        plot4(x, y);
    }

    // Region 2 (slope < -1)
    double p2 = (ry2 * (x + 0.5) * (x + 0.5)) + (rx2 * (y - 1) * (y - 1)) - (rx2 * ry2);
    while (y > 0) {
        y--;
        py -= twoRx2;
        if (p2 > 0) {
            p2 += rx2 - py;
        } else {
            x++;
            px += twoRy2;
            p2 += rx2 - py + px;
        }
        plot4(x, y);
    }
}

// --- 5. Parametric Cubic Bézier Curve ---

void SoftwareCanvas::drawBezier(const Point2D& p0, const Point2D& p1,
                                const Point2D& p2, const Point2D& p3,
                                const Color& color) {
    // Number of line segments to approximate curve smoothly
    constexpr int SEGMENTS = 48;
    Point2D prevPt = p0;

    for (int i = 1; i <= SEGMENTS; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(SEGMENTS);
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        // B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
        float bx = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
        float by = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;

        Point2D curPt(static_cast<int>(std::round(bx)), static_cast<int>(std::round(by)));
        drawLine(prevPt.x, prevPt.y, curPt.x, curPt.y, color);
        prevPt = curPt;
    }
}

// --- 6. Text Rendering (8x8 Bitmap Font) ---

void SoftwareCanvas::drawText(int x, int y, const std::string& text, const Color& color) {
    int curX = x;
    int curY = y;

    for (char c : text) {
        if (c == '\n') {
            curY += 10;
            curX = x;
            continue;
        }

        // ASCII 32 (' ') to 126 ('~')
        if (c >= 32 && c <= 126) {
            int glyphIdx = c - 32;
            for (int row = 0; row < 8; ++row) {
                uint8_t rowBits = FONT_8X8[glyphIdx][row];
                for (int col = 0; col < 8; ++col) {
                    // Check bit from left to right (MSB to LSB)
                    if ((rowBits >> (7 - col)) & 0x01) {
                        drawPixel(curX + col, curY + row, color);
                    }
                }
            }
        }
        curX += 8; // Advance character width
    }
}

// --- 7. BMP Exporter (Uncompressed 24-bit TrueColor) ---

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType{0x4D42}; // "BM"
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54};
};

struct BMPInfoHeader {
    uint32_t size{40};
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};
    uint16_t bitCount{24};
    uint32_t compression{0};
    uint32_t sizeImage{0};
    int32_t  xPixelsPerMeter{0};
    int32_t  yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)

bool SoftwareCanvas::saveToBMP(const std::string& filepath) const {
    if (m_width <= 0 || m_height <= 0 || m_pixels.empty()) return false;

    std::ofstream out(filepath, std::ios::binary);
    if (!out) return false;

    // Rows must be padded to a multiple of 4 bytes
    int rowPadding = (4 - ((m_width * 3) % 4)) % 4;
    uint32_t rowStride = m_width * 3 + rowPadding;
    uint32_t imageSize = rowStride * m_height;

    BMPHeader fileHeader;
    fileHeader.fileSize = 54 + imageSize;

    BMPInfoHeader infoHeader;
    infoHeader.width = m_width;
    infoHeader.height = m_height; // Positive = bottom-up DIB
    infoHeader.sizeImage = imageSize;

    out.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    out.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

    std::vector<uint8_t> paddingBytes(rowPadding, 0);

    // BMP is stored bottom-to-top
    for (int y = m_height - 1; y >= 0; --y) {
        for (int x = 0; x < m_width; ++x) {
            const Color& c = m_pixels[y * m_width + x];
            // Format is BGR
            uint8_t bgr[3] = { c.b, c.g, c.r };
            out.write(reinterpret_cast<const char*>(bgr), 3);
        }
        if (rowPadding > 0) {
            out.write(reinterpret_cast<const char*>(paddingBytes.data()), rowPadding);
        }
    }

    return true;
}

bool SoftwareCanvas::saveToPPM(const std::string& filepath) const {
    if (m_width <= 0 || m_height <= 0 || m_pixels.empty()) return false;

    std::ofstream out(filepath, std::ios::binary);
    if (!out) return false;

    out << "P6\n" << m_width << " " << m_height << "\n255\n";
    for (const auto& c : m_pixels) {
        uint8_t rgb[3] = { c.r, c.g, c.b };
        out.write(reinterpret_cast<const char*>(rgb), 3);
    }
    return true;
}

} // namespace atomica::ui
