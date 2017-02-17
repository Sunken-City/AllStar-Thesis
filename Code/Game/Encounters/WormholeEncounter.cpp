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
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    nebby->m_sprite->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    nebby->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(nebby);
}

