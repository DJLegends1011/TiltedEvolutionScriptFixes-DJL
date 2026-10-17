#pragma once

#include <SkyrimTogetherAPI.h>

struct NotifyModMessage;

/**
 * @brief The Plugin API that SKSE plugins use to sync their own state through Skyrim Together.
 *
 * Not a World service, as plugins may ask for the interface before the World exists. The World forwards
 * NotifyModMessage to it.
 */
struct PluginService final : SkyrimTogetherAPI::IVSkyrimTogether1
{
    [[nodiscard]] static PluginService& Get() noexcept;

    TP_NOCOPYMOVE(PluginService);

    SkyrimTogetherAPI::ChannelHandle
    RegisterChannel(const char* apName, SkyrimTogetherAPI::MessageCallback aOnMessage, SkyrimTogetherAPI::PartyChangedCallback aOnPartyChanged) noexcept override;
    SkyrimTogetherAPI::SendResult Send(SkyrimTogetherAPI::ChannelHandle aChannel, const char* apBytes, size_t aLength) noexcept override;
    SkyrimTogetherAPI::SendResult SendTo(SkyrimTogetherAPI::ChannelHandle aChannel, uint32_t aPlayerId, const char* apBytes, size_t aLength) noexcept override;
    uint32_t GetLocalPlayerId() const noexcept override;
    bool IsInParty() const noexcept override;

    /**
     * @brief Hands a relayed Mod Message to the plugin on its channel, if any. Runs on the main thread.
     */
    void OnNotifyModMessage(const NotifyModMessage& acMessage) noexcept;

private:
    PluginService() = default;

    struct Channel
    {
        String Name;
        SkyrimTogetherAPI::MessageCallback OnMessage;
        SkyrimTogetherAPI::PartyChangedCallback OnPartyChanged;
    };

    // Validates on the calling thread, then sends from the main thread. aTargetPlayerId 0 means the whole party.
    SkyrimTogetherAPI::SendResult Queue(SkyrimTogetherAPI::ChannelHandle aChannel, uint32_t aTargetPlayerId, const char* apBytes, size_t aLength) noexcept;

    mutable std::mutex m_lock;
    // A handle is the index in here plus one
    Vector<Channel> m_channels;
};
