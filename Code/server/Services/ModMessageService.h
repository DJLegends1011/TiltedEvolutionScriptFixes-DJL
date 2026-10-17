#pragma once

#include <Events/PacketEvent.h>

struct World;
struct SendModMessageRequest;

/**
 * @brief Relays Mod Messages sent through the Plugin API. The payload is never interpreted, only length-checked.
 */
struct ModMessageService
{
    ModMessageService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~ModMessageService() noexcept = default;

    TP_NOCOPYMOVE(ModMessageService);

protected:
    void OnSendModMessage(const PacketEvent<SendModMessageRequest>& acMessage) const noexcept;

private:
    World& m_world;

    entt::scoped_connection m_sendModMessageConnection;
};
