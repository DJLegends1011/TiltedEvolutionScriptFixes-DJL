#pragma once

#include "Message.h"
#include "../Opcodes.h"

struct NotifyWaitTimeResult final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyWaitTimeResult;

    NotifyWaitTimeResult()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyWaitTimeResult& achRhs) const noexcept { return GetOpcode() == achRhs.GetOpcode() && Result == achRhs.Result; }

    enum class WaitTimeResult : uint8_t
    {
        kSuccess,
        kNotOldestLeader
    };

    WaitTimeResult Result{};
};
