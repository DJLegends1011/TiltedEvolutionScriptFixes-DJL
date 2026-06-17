#include "WaitTimeCommandRequest.h"

void WaitTimeCommandRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(static_cast<uint64_t>(Hours), 32);
    aWriter.WriteBits(static_cast<uint64_t>(Minutes), 32);
    aWriter.WriteBits(static_cast<uint64_t>(PlayerId), 32);
}

void WaitTimeCommandRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t value = 0;

    aReader.ReadBits(value, 32);
    Hours = static_cast<uint32_t>(value);

    aReader.ReadBits(value, 32);
    Minutes = static_cast<uint32_t>(value);

    aReader.ReadBits(value, 32);
    PlayerId = static_cast<uint32_t>(value);
}
