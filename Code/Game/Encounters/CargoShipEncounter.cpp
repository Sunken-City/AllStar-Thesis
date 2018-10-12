#include "Game/Encounters/CargoShipEncounter.hpp"
#include "../Entities/Enemies/Grunt.hpp"
#include "../Entities/Props/ItemCrate.hpp"
#include "../Entities/Enemies/Brute.hpp"
#include "../Entities/Enemies/Turret.hpp"

//-----------------------------------------------------------------------------------
CargoShipEncounter::CargoShipEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void CargoShipEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();
    const int NUM_CRATES = MathUtils::GetRandomInt(3, 5);

    for (int i = 0; i < NUM_CRATES; ++i)
    {
        float x = MathUtils::GetRandomFloat(-0.3f, 0.3f);
        float y = MathUtils::GetRandomFloat(-0.3f, 0.3f);
        Vector2 spawnPos = Vector2(x, y);
        gameMode->SpawnEntityInGameWorld(new ItemCrate(CalculateSpawnPosition(spawnPos)));
    }

    if (MathUtils::CoinFlip())
    {
        gameMode->SpawnEntityInGameWorld(new Turret(CalculateSpawnPosition(Vector2(0.0f, 1.0f))));
        gameMode->SpawnEntityInGameWorld(new Turret(CalculateSpawnPosition(Vector2(0.0f, -1.0f))));
        gameMode->SpawnEntityInGameWorld(new Turret(CalculateSpawnPosition(Vector2(1.0f, 0.0f))));
        gameMode->SpawnEntityInGameWorld(new Turret(CalculateSpawnPosition(Vector2(-1.0f, 0.0f))));
    }
    else
    {
        gameMode->SpawnEntityInGameWorld(new Brute(CalculateSpawnPosition(Vector2(0.0f, 1.0f))));
        gameMode->SpawnEntityInGameWorld(new Brute(CalculateSpawnPosition(Vector2(0.0f, -1.0f))));
    }
}

