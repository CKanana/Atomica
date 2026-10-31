// Mat4.h — the 3D maths for the engine, written by hand (no GLM).
// COLUMN-MAJOR like OpenGL: element (row r, col c) = m[c*4 + r].
// Points are column vectors, so  p' = M * p  and  MVP = Projection * View * Model
// (the Model matrix is applied to the point first).
#pragma once
#include <cmath>

struct Vec3 {
    float x = 0, y = 0, z = 0;
};

struct Mat4 {
    float m[16];

    static Mat4 identity() {
        Mat4 r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    // C = A * B  (apply B first, then A)
    friend Mat4 operator*(const Mat4& A, const Mat4& B) {
        Mat4 C{};
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r) {
                float s = 0.0f;
                for (int k = 0; k < 4; ++k) s += A.m[k * 4 + r] * B.m[c * 4 + k];
                C.m[c * 4 + r] = s;
            }
        return C;
    }

    static Mat4 translate(float x, float y, float z) {
        Mat4 r = identity();
        r.m[12] = x; r.m[13] = y; r.m[14] = z;
        return r;
    }

    // rotation about X:  [1 0 0; 0 c -s; 0 s c]
    static Mat4 rotateX(float a) {
        Mat4 r = identity();
        float c = std::cos(a), s = std::sin(a);
        r.m[5] = c;  r.m[9]  = -s;
        r.m[6] = s;  r.m[10] = c;
        return r;
    }

    // rotation about Y:  [c 0 s; 0 1 0; -s 0 c]
    static Mat4 rotateY(float a) {
        Mat4 r = identity();
        float c = std::cos(a), s = std::sin(a);
        r.m[0] = c;  r.m[8]  = s;
        r.m[2] = -s; r.m[10] = c;
        return r;
    }

    // Right-handed perspective projection, clip-space z in [-1, 1] (like gluPerspective).
    static Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) {
        Mat4 r{};
        float f = 1.0f / std::tan(fovyRad * 0.5f);
        r.m[0]  = f / aspect;
        r.m[5]  = f;
        r.m[10] = (zFar + zNear) / (zNear - zFar);
        r.m[11] = -1.0f;                                // w' = -z  -> perspective divide
        r.m[14] = (2.0f * zFar * zNear) / (zNear - zFar);
        return r;
    }

    // Transform a point (w = 1). Valid for rotation/translation matrices (affine).
    Vec3 transformPoint(const Vec3& p) const {
        return {m[0] * p.x + m[4] * p.y + m[8]  * p.z + m[12],
                m[1] * p.x + m[5] * p.y + m[9]  * p.z + m[13],
                m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14]};
    }
};
