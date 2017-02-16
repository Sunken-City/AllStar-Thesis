#include "Game/Encounters/WormholeEncounter.hpp"
#include "../Entities/Props/Wormhole.hpp"

//-----------------------------------------------------------------------------------
WormholeEncounter::WormholeEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void WormholeEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();

    Wormhole* nebby = new Wormhole(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));
    AABB2 bounds = nebby->m_sprite->m_spriteResource->GetDefaultBounds();
    nebby->m_sprite->m_transform.SetScale(Vector2(m_radius / bounds.GetWidth(), m_radius / bounds.GetHeight()));
    nebby->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(nebby);
}

