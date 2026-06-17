#include "WaitTimeCommandRequest.h"

void WaitTimeCommandRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.Write(Hours);
    aWriter.Write(Minutes);
    aWriter.Write(PlayerId);
}

void WaitTimeCommandRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    aReader.Read(Hours);
    aReader.Read(Minutes);
    aReader.Read(PlayerId);
}
