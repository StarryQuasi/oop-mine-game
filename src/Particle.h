#pragma once

#include <chrono>

#include "libs/olcPixelGameEngine.h"

enum class ParticleType : uint8_t
{
    generic,
};

struct Particle
{
    std::chrono::steady_clock::time_point lifeStart{};
    std::chrono::steady_clock::time_point lifeEnd{};
    olc::vf2d pos{};
    olc::vf2d vel{};
    olc::Pixel color{};
    // Scale of 32 = 1 block
    float scale{};
    bool alive{};
    ParticleType type{};
};