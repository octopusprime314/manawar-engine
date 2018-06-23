#include "Noise.h"

#include <cmath>

// Return a random float in [0, 1] which changes smoothly in x and y.
// Effectively 'Value Noise'.
float ValueNoise2D::noise(float x, float y) {
    uint32_t w = 128;
    uint32_t h = 128;

    // modf requires a pointer argument, and it must not be NULL.
    float dummy = 0;

    float    xf = modf(x, &dummy);
    uint32_t x1 = (uint32_t(x) + w) % w;
    uint32_t x2 = (x1 + w - 1) % w;

    float    yf = modf(y, &dummy);
    uint32_t y1 = (uint32_t(y) + h) % h;
    uint32_t y2 = (y1 + h - 1) % h;

    return bilerp(xf, yf,
        data[x1][y1],
        data[x1][y2],
        data[x2][y1],
        data[x2][y2]);
}

float ValueNoise2D::turbulence(float x, float y, uint8_t octaves) {
    float val = 0.f;
    for (auto w = PowerOfTwo<uint64_t>(octaves); w >= 1; w /= 2) {
        val += w * noise(x / w, y / w);
    }
    val /= PowerOfTwo<float>(octaves + 1);

    return val;
}

// This noise data matches the original data from its Python roots.
// The goal is to replicate the maps verbatim, at least for testing.
#include "NoiseData.h"
ValueNoise2D kNoise(kValueNoise2DPythonData);
