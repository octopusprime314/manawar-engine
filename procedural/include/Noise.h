#pragma once

#include <stdint.h>
#include <string.h>

// TODO: Get python rand?
// Useful for finding new seeds.
// s = r.randint(0, 0x1000)
// print(hex(s))
static constexpr uint32_t kRngSeed = 0x8d1;

template <typename T>
T lerp(T t,
       T lo,
       T hi)
{
    return t * lo + (1 - t)*hi;
}

template <typename T>
T bilerp(T t0,
         T t1,
         T q00,
         T q01,
         T q10,
         T q11)
{
    T r0 = lerp(t0,
                q00,
                q10);
    T r1 = lerp(t0,
                q01,
                q11);
    return lerp(t1,
                r0,
                r1);
}

template <typename T>
T PowerOfTwo(uint8_t power)
{
    uint64_t res = 1;
    return static_cast<T>(res << power);
}

struct ValueNoise2D {
    float data[128][128];

    explicit ValueNoise2D(const float data[128][128])
    {
        memcpy(this->data, data, sizeof(this->data));
    }

    ~ValueNoise2D() = default;

    float noise(     float   x,
                     float   y);
    float turbulence(float   x,
                     float   y,
                     uint8_t octaves);
};
extern ValueNoise2D kNoise;
