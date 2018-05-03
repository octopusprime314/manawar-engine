#pragma once

#include <stdint.h>
#include <string.h>

// TODO: Get python rand?
// Useful for finding new seeds.
// s = r.randint(0, 0x1000)
// print(hex(s))
static constexpr uint32_t kRngSeed = 0x8d1;

struct ValueNoise2D {
    float data[128][128];

    // Don't use this - it's not implemented yet.
    explicit ValueNoise2D(uint32_t seed = 0x8d1);

    explicit ValueNoise2D(const float data[128][128])
    {
        memcpy(this->data, data, sizeof(this->data));
    }

    ~ValueNoise2D() = default;

    float noise(float x, float y);
    float turbulence(float x, float y, uint8_t octaves);
};

extern ValueNoise2D kNoise;
