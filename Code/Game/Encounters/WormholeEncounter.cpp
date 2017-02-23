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

    m_spawnedWormhole = new Wormhole(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));

    AABB2 bounds = m_spawnedWormhole->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    m_spawnedWormhole->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    m_spawnedWormhole->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(m_spawnedWormhole);
    if (m_linkedWormholeEncounter)
    {
        Wormhole::LinkWormholes(m_spawnedWormhole, m_linkedWormholeEncounter->m_spawnedWormhole);
    }
}

//-----------------------------------------------------------------------------------
Encounter* WormholeEncounter::CreateLinkedEncounter(const Vector2& center, float radius)
{
    WormholeEncounter* other = new WormholeEncounter(center, radius);
    WormholeEncounter::LinkWormholes(this, other);
    return other;
}

//-----------------------------------------------------------------------------------
void WormholeEncounter::LinkWormholes(WormholeEncounter* wormhole1, WormholeEncounter* wormhole2)
{
    wormhole1->m_linkedWormholeEncounter = wormhole2;
    wormhole2->m_linkedWormholeEncounter = wormhole1;
}

