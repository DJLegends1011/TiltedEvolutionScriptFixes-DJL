#pragma once

#include <cstdint>

struct WaitTimeCommandEvent
{
    WaitTimeCommandEvent() = delete;
    WaitTimeCommandEvent(uint8_t aHours, uint8_t aMinutes, uint32_t aPlayerId)
        : Hours(aHours)
        , Minutes(aMinutes)
        , PlayerId(aPlayerId)
    {
    }

    uint8_t Hours;
    uint8_t Minutes;
    uint32_t PlayerId;
};
