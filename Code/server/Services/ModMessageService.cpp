#include <Services/ModMessageService.h>
#include <GameServer.h>

#include <Messages/ModMessage.h>
#include <Messages/NotifyModMessage.h>
#include <Messages/SendModMessageRequest.h>

#include <Game/Player.h>
#include <World.h>

ModMessageService::ModMessageService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_sendModMessageConnection = aDispatcher.sink<PacketEvent<SendModMessageRequest>>().connect<&ModMessageService::OnSendModMessage>(this);
}

void ModMessageService::OnSendModMessage(const PacketEvent<SendModMessageRequest>& acMessage) const noexcept
{
    const auto& cRequest = acMessage.Packet;

    // Clients are not trusted: a modded one can send anything, so drop whatever the Plugin API would not have sent.
    // The per-player rate limit goes here too.
    if (!ModMessage::IsValidChannelName(cRequest.Channel) || cRequest.Data.size() > SkyrimTogetherAPI::kMaxMessageSize)
        return;

    const auto& cParty = acMessage.pPlayer->GetParty();
    if (!cParty.JoinedPartyId.has_value())
        return;

    NotifyModMessage notify{};
    notify.Channel = cRequest.Channel;
    notify.SenderPlayerId = acMessage.pPlayer->GetId();
    notify.Data = cRequest.Data;

    if (cRequest.TargetPlayerId == 0)
    {
        GameServer::Get()->SendToParty(notify, cParty, acMessage.GetSender());
        return;
    }

    // The target must be in the sender's own party, whatever the client claims
    const Player* pTarget = m_world.GetPlayerManager().GetById(cRequest.TargetPlayerId);
    if (!pTarget || pTarget == acMessage.pPlayer || pTarget->GetParty().JoinedPartyId != cParty.JoinedPartyId)
        return;

    pTarget->Send(notify);
}
