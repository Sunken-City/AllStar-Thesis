#include "Game/Encounters/NebulaEncounter.hpp"
#include "../Entities/Props/Nebula.hpp"

//-----------------------------------------------------------------------------------
NebulaEncounter::NebulaEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void NebulaEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();

    Nebula* nebby = new Nebula(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));

    AABB2 bounds = nebby->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    nebby->m_sprite->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    nebby->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(nebby);
}

