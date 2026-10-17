#include <Services/PluginService.h>

#include <World.h>

#include <Messages/ModMessage.h>
#include <Messages/NotifyModMessage.h>
#include <Messages/SendModMessageRequest.h>

using namespace SkyrimTogetherAPI;

PluginService& PluginService::Get() noexcept
{
    static PluginService s_instance;
    return s_instance;
}

ChannelHandle PluginService::RegisterChannel(const char* apName, MessageCallback aOnMessage, PartyChangedCallback aOnPartyChanged) noexcept
{
    // Reading up to one byte past the limit is enough to tell the name is too long
    const String cName = apName ? String(apName, strnlen(apName, kMaxChannelNameSize + 1)) : String();
    if (!ModMessage::IsValidChannelName(cName))
        return 0;

    std::lock_guard cLock(m_lock);

    for (const Channel& cChannel : m_channels)
    {
        if (cChannel.Name == cName)
            return 0;
    }

    m_channels.push_back({cName, aOnMessage, aOnPartyChanged});
    return static_cast<ChannelHandle>(m_channels.size());
}

SendResult PluginService::Send(ChannelHandle aChannel, const char* apBytes, size_t aLength) noexcept
{
    return Queue(aChannel, 0, apBytes, aLength);
}

SendResult PluginService::SendTo(ChannelHandle aChannel, uint32_t aPlayerId, const char* apBytes, size_t aLength) noexcept
{
    // Not supported yet
    return SendResult::NotInParty;
}

uint32_t PluginService::GetLocalPlayerId() const noexcept
{
    if (!entt::locator<World>::has_value() || !World::Get().GetTransport().IsOnline())
        return 0;

    return World::Get().GetTransport().GetLocalPlayerId();
}

bool PluginService::IsInParty() const noexcept
{
    return entt::locator<World>::has_value() && World::Get().GetPartyService().IsInParty();
}

SendResult PluginService::Queue(ChannelHandle aChannel, uint32_t aTargetPlayerId, const char* apBytes, size_t aLength) noexcept
{
    String channelName;
    {
        std::lock_guard cLock(m_lock);

        if (aChannel == 0 || aChannel > m_channels.size())
            return SendResult::BadChannel;

        channelName = m_channels[aChannel - 1].Name;
    }

    if (aLength > kMaxMessageSize)
        return SendResult::TooLarge;

    if (!entt::locator<World>::has_value() || !World::Get().GetTransport().IsOnline())
        return SendResult::NotConnected;

    if (!World::Get().GetPartyService().IsInParty())
        return SendResult::NotInParty;

    // The state may have changed by the time the main thread gets to it, in which case the message is dropped
    World::Get().GetRunner().Queue(
        [channelName = std::move(channelName), aTargetPlayerId, data = String(apBytes ? apBytes : "", apBytes ? aLength : 0)]()
        {
            auto& world = World::Get();
            if (!world.GetTransport().IsOnline() || !world.GetPartyService().IsInParty())
                return;

            SendModMessageRequest request{};
            request.Channel = channelName;
            request.TargetPlayerId = aTargetPlayerId;
            request.Data = data;
            world.GetTransport().Send(request);
        });

    return SendResult::Ok;
}

void PluginService::OnNotifyModMessage(const NotifyModMessage& acMessage) noexcept
{
    MessageCallback onMessage = nullptr;
    {
        std::lock_guard cLock(m_lock);

        for (const Channel& cChannel : m_channels)
        {
            if (cChannel.Name == acMessage.Channel)
            {
                onMessage = cChannel.OnMessage;
                break;
            }
        }
    }

    // No plugin registered that channel on this client: nothing to do
    if (onMessage)
        onMessage(acMessage.SenderPlayerId, acMessage.Data.data(), acMessage.Data.size());
}
