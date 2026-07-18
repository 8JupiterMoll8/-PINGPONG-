#pragma once

#include <cstdint>

namespace pingpong {

constexpr std::uint8_t protocolVersion{1};

enum class SystemStatus : std::uint8_t {
    booting = 0,
    running = 1,
    technicalPause = 2,
};

// Raw bytes sent by EasyTransfer. Keep naturally aligned 32-bit values first
// and update protocolVersion whenever this layout changes.
struct __attribute__((packed)) WorldFrame {
    std::uint8_t protocolVersion{pingpong::protocolVersion};
    std::uint8_t systemStatus{static_cast<std::uint8_t>(SystemStatus::booting)};
    std::uint16_t frameSequence{0};
    float leftRacketRoll{0.0F};
    float rightRacketSpeed{0.0F};
    std::uint8_t leftRacketHitCount{0};
    std::uint8_t rightRacketHitCount{0};
    std::uint8_t leftTableHitCount{0};
    std::uint8_t rightTableHitCount{0};
};

static_assert(sizeof(float) == 4, "The protocol requires 32-bit floats");
static_assert(sizeof(WorldFrame) == 16, "Update protocolVersion when WorldFrame changes");

constexpr bool isCompatible(const WorldFrame& frame) noexcept
{
    return frame.protocolVersion == protocolVersion;
}

constexpr bool isRunning(const WorldFrame& frame) noexcept
{
    return frame.systemStatus == static_cast<std::uint8_t>(SystemStatus::running);
}

} // namespace pingpong
