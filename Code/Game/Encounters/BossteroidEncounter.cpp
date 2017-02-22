#include "Game/Encounters/BossteroidEncounter.hpp"
#include "../Entities/Props/Asteroid.hpp"

//-----------------------------------------------------------------------------------
BossteroidEncounter::BossteroidEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void BossteroidEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();

    Asteroid* asteroid = new Asteroid(CalculateSpawnPosition(Vector2::ZERO));

    AABB2 bounds = asteroid->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    asteroid->m_sprite->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    asteroid->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    asteroid->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(asteroid);
}

