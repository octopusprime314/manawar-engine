#include <math.h>

#include "Noise.h"

#include <string.h>

template <typename T>
T lerp(T t, T lo, T hi) {
    return t*lo + (1 - t)*hi;
}

template <typename T>
T bilerp(T t0, T t1, T q00, T q01, T q10, T q11) {
    T r0 = lerp(t0, q00, q10);
    T r1 = lerp(t0, q01, q11);
    return lerp(t1, r0, r1);
}

// Return a random float in [0, 1] which changes smoothly in x and y.
// Effectively 'Value Noise'.
float Noise::noise(float x, float y) {
    uint32_t w = 128;
    uint32_t h = 128;

    float    xf = x - uint32_t(x);
    uint32_t x1 = (uint32_t(x) + w) % w;
    uint32_t x2 = (x1 + w - 1) % w;

    float    yf = y - uint32_t(y);
    uint32_t y1 = (uint32_t(y) + h) % h;
    uint32_t y2 = (y1 + h - 1) % h;

    return bilerp(xf, yf,
                  data[x1][y1],
                  data[x1][y2],
                  data[x2][y1],
                  data[x2][y2]);
}

float Noise::turbulence(float x, float y, uint8_t octaves) {
    float val = 0;
    uint64_t w0 = (1ull << octaves);
    uint64_t w  = (1ull << octaves);


    while (w >= 1) {
        val += w * noise(x/w, y/w);
        w /= 2;
    }

    return val / (w0 * 2);
}

#include "NoiseData.h"
Noise kNoise(kData);
