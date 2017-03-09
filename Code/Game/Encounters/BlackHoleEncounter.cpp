#include "Game/Encounters/BlackHoleEncounter.hpp"
#include "../Entities/Props/BlackHole.hpp"

//-----------------------------------------------------------------------------------
BlackHoleEncounter::BlackHoleEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void BlackHoleEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();
    m_spawnedBlackHole = new BlackHole(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));

    AABB2 bounds = m_spawnedBlackHole->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    m_spawnedBlackHole->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    m_spawnedBlackHole->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(m_spawnedBlackHole);
}