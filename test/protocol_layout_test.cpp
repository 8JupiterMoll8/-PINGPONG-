#include <cassert>
#include <cstddef>
#include <cstdint>

#include "PingPongProtocol.h"

int main()
{
    using pingpong::SystemStatus;
    using pingpong::WorldFrame;

    static_assert(sizeof(float) == 4, "The wire protocol requires 32-bit floats");
    static_assert(sizeof(WorldFrame) == 16, "WorldFrame wire size changed");
    static_assert(offsetof(WorldFrame, protocolVersion) == 0, "Version must be first");
    static_assert(offsetof(WorldFrame, frameSequence) == 2, "Sequence offset changed");
    static_assert(offsetof(WorldFrame, leftRacketRoll) == 4, "Left roll offset changed");
    static_assert(offsetof(WorldFrame, rightRacketSpeed) == 8, "Right speed offset changed");
    static_assert(offsetof(WorldFrame, leftRacketHitCount) == 12, "Left racket offset changed");
    static_assert(offsetof(WorldFrame, rightRacketHitCount) == 13, "Right racket offset changed");

    WorldFrame frame{};
    assert(frame.protocolVersion == pingpong::protocolVersion);
    assert(frame.systemStatus == static_cast<std::uint8_t>(SystemStatus::booting));
    assert(pingpong::isCompatible(frame));

    frame.protocolVersion++;
    assert(!pingpong::isCompatible(frame));
}
