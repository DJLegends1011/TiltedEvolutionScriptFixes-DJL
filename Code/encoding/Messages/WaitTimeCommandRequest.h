#pragma once

#include "Message.h"

struct WaitTimeCommandRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kWaitTimeCommandRequest;

    WaitTimeCommandRequest() = default;
    WaitTimeCommandRequest(uint32_t aHours, uint32_t aMinutes, uint32_t aPlayerId)
        : ClientMessage(kWaitTimeCommandRequest)
        , Hours(aHours)
        , Minutes(aMinutes)
        , PlayerId(aPlayerId)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t Hours{};
    uint32_t Minutes{};
    uint32_t PlayerId{};
};
