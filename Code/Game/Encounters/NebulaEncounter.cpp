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
    nebby->m_sprite->m_transform.SetScale(Vector2(m_radius / bounds.GetWidth(), m_radius / bounds.GetHeight()));

    gameMode->SpawnEntityInGameWorld(nebby);
}

