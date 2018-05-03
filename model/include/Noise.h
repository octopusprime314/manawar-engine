#pragma once

#include <stdint.h>

// TODO: Get python rand?
// Useful for finding new seeds.
// s = r.randint(0, 0x1000)
// print(hex(s))
static constexpr uint32_t kRngSeed = 0x8d1;

struct Noise {
    float data[128][128];

    // Don't use this - it's not implemented yet.
    explicit Noise(uint32_t seed = 0x8d1);

    explicit Noise(const float data[128][128])
    {
        memcpy(this->data, data, sizeof(this->data));
    }

    ~Noise() = default;

    float noise(float x, float y);
    float turbulence(float x, float y, uint8_t octaves);
};

extern Noise kNoise;
