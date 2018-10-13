#include "Game/Encounters/HealingZoneEncounter.hpp"
#include "../Entities/Props/HealingZone.hpp"
#include "Engine/Math/Dice.hpp"
#include "../Entities/Props/ItemCrate.hpp"
#include "../Entities/Enemies/Grunt.hpp"
#include "../GameModes/AssemblyMode.hpp"

//-----------------------------------------------------------------------------------
HealingZoneEncounter::HealingZoneEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void HealingZoneEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();

    HealingZone* healingZone = new HealingZone(CalculateSpawnPosition(Vector2(0.0f, 0.0f)));

    AABB2 bounds = healingZone->m_sprite->m_spriteResource->GetDefaultBounds();
    Vector2 normalizingScale = Vector2(2.0f / bounds.GetWidth(), 2.0f / bounds.GetHeight()); //Makes our scale for the object 1:1 so we can multiply by the radius
    healingZone->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    healingZone->CalculateCollisionRadius();

    gameMode->SpawnEntityInGameWorld(healingZone);
}

