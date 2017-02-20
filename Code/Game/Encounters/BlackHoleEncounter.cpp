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
    BlackHole* spawnedBlackHole = new BlackHole(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));

    AABB2 bounds = spawnedBlackHole->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    spawnedBlackHole->m_sprite->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    spawnedBlackHole->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(spawnedBlackHole);
}