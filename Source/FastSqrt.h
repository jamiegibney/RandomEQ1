// Approximations of square roots for increased performance
// In the case of this application they are negligible, but here for future use
#pragma once

struct FastSqrt {
    // 1 babylonian step (least accurate, ~2x faster than sqrt())
    static double FS1(const double& x) {
        double v = Shift(x);
        v = 0.5 * (v + x / v);
        return v;
    }

    // 2 babylonian steps (more accurate, ~50% faster than sqrt())
    static double FS2(const double& x) {
        double v = Shift(x);
        v += x / v;
        v = 0.25 * (v + x / v);
        return v;
    }

 private:
    // bit-shifting operation
    static inline double Shift(const double& in) {
        union {long i; double x; } u {};
        u.x = in;
        u.i = (long(1) << 61) + (u.i >> 1) - (long(1) << 51);
        return u.x;
    }
};