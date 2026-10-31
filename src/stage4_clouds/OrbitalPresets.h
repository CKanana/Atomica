// The orbitals the N / B keys cycle through (n, l, m).
#pragma once
struct OrbitalPreset { int n, l, m; };

inline const OrbitalPreset* orbitalPresets(int& count) {
    static const OrbitalPreset kPresets[] = {
        {1, 0, 0},   // 1s
        {2, 0, 0},   // 2s  (radial node)
        {2, 1, 0},   // 2p_z
        {2, 1, 1},   // 2p_x
        {3, 2, 0},   // 3d_z2
        {3, 2, 1},   // 3d_xz
        {3, 2, 2},   // 3d_x2-y2
        {4, 3, 0},   // 4f
    };
    count = static_cast<int>(sizeof kPresets / sizeof kPresets[0]);
    return kPresets;
}
