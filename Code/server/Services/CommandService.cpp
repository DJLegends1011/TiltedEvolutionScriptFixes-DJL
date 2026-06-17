#include <Services/CommandService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Messages/SetTimeCommandRequest.h>
#include <Messages/NotifySetTimeResult.h>
#include <Messages/TeleportCommandRequest.h>
#include <Messages/TeleportCommandResponse.h>
#include <Messages/WaitTimeCommandRequest.h>
#include <Messages/NotifyWaitTimeResult.h>

#include <PartyService.h>

CommandService::CommandService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_setTimeConnection = aDispatcher.sink<PacketEvent<SetTimeCommandRequest>>().connect<&CommandService::OnSetTimeCommand>(this);
    m_waitTimeConnection = aDispatcher.sink<PacketEvent<WaitTimeCommandRequest>>().connect<&CommandService::OnWaitTimeCommand>(this);
    m_teleportConnection = aDispatcher.sink<PacketEvent<TeleportCommandRequest>>().connect<&CommandService::OnTeleportCommandRequest>(this);
}

void CommandService::OnSetTimeCommand(const PacketEvent<SetTimeCommandRequest>& acMessage) const noexcept
{
    NotifySetTimeResult response{};

    const auto cPlayerId = static_cast<uint32_t>(acMessage.Packet.PlayerId);

    // Only set time if player is an admin
    for (const auto session : GameServer::Get()->GetAdminSessions())
    {
        if (PlayerManager::Get()->GetByConnectionId(session)->GetId() == cPlayerId)
        {
            const auto cHours = static_cast<int>(acMessage.Packet.Hours);
            const auto cMinutes = static_cast<int>(acMessage.Packet.Minutes);

            m_world.GetCalendarService().SetTime(cHours, cMinutes, m_world.GetCalendarService().GetTimeScale());

            response.Result = NotifySetTimeResult::SetTimeResult::kSuccess;
            acMessage.pPlayer->Send(response);

            return;
        }
    }

    response.Result = NotifySetTimeResult::SetTimeResult::kNoPermission;
    acMessage.pPlayer->Send(response);
}

void CommandService::OnWaitTimeCommand(const PacketEvent<WaitTimeCommandRequest>& acMessage) const noexcept
{
    NotifyWaitTimeResult response{};

    const auto cPlayerId = static_cast<uint32_t>(acMessage.Packet.PlayerId);
    const auto cHours = static_cast<int>(acMessage.Packet.Hours);

    const auto* pPartyService = static_cast<PartyService*>(&m_world.GetPartyService());
    if (pPartyService->GetOldestLeaderId() == cPlayerId)
    {
        m_world.GetCalendarService().SetTime(cHours, 0, m_world.GetCalendarService().GetTimeScale());

        response.Result = NotifyWaitTimeResult::WaitTimeResult::kSuccess;
        acMessage.pPlayer->Send(response);

        return;
    }

    response.Result = NotifyWaitTimeResult::WaitTimeResult::kNotOldestLeader;
    acMessage.pPlayer->Send(response);
}

void CommandService::OnTeleportCommandRequest(const PacketEvent<TeleportCommandRequest>& acMessage) const noexcept
{
    Player* pTargetPlayer = nullptr;
    for (Player* pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer->GetUsername() == acMessage.Packet.TargetPlayer)
            pTargetPlayer = pPlayer;
    }

    TeleportCommandResponse response{};
    if (pTargetPlayer)
    {
        auto character = pTargetPlayer->GetCharacter();
        if (character)
        {
            const auto* pMovementComponent = m_world.try_get<MovementComponent>(*character);
            if (pMovementComponent)
            {
                const auto& cellComponent = pTargetPlayer->GetCellComponent();
                response.CellId = cellComponent.Cell;
                response.Position = pMovementComponent->Position;
                response.WorldSpaceId = cellComponent.WorldSpaceId;
            }
        }
    }

    acMessage.pPlayer->Send(response);
}
