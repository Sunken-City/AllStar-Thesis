#include "Game/Encounters/NebulaEncounter.hpp"
#include "../Entities/Props/Nebula.hpp"
#include "Engine/Math/Dice.hpp"
#include "../Entities/Props/ItemCrate.hpp"
#include "../Entities/Grunt.hpp"

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
    nebby->m_transform.SetScale(normalizingScale * Vector2(m_radius));
    nebby->CalculateCollisionRadius();

    Dice nebulaItemDie(1, 4);
    int dieRoll = nebulaItemDie.Roll();
    if (dieRoll == 1)
    {
        float x = MathUtils::GetRandomFloat(-0.5f, 0.5f);
        float y = MathUtils::GetRandomFloat(-0.5f, 0.5f);
        Vector2 spawnPos = Vector2(x, y);
        gameMode->SpawnEntityInGameWorld(new ItemCrate(CalculateSpawnPosition(spawnPos)));
    }
    else if (dieRoll == 2)
    {
        float x = MathUtils::GetRandomFloat(-0.5f, 0.5f);
        float y = MathUtils::GetRandomFloat(-0.5f, 0.5f);
        Vector2 spawnPos = Vector2(x, y);
        gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(spawnPos)));
    }

    gameMode->SpawnEntityInGameWorld(nebby);
}

