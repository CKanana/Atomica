#pragma once

#include <cstdint>

namespace atomica::ui {

/**
 * 8x8 Monochrome Bitmap Font for printable ASCII characters (32..126).
 * Pure C++ embedded font allowing text rendering without external dependencies.
 * Each character consists of 8 bytes (8 rows of 8 horizontal 1-bit pixels).
 */
extern const uint8_t FONT_8X8[95][8];

} // namespace atomica::ui
