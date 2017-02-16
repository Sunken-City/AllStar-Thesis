#include "Game/Encounters/SquadronEncounter.hpp"
#include "../Entities/Grunt.hpp"

//-----------------------------------------------------------------------------------
SquadronEncounter::SquadronEncounter(const Vector2& center, float radius)
    : Encounter(center, radius)
{

}

//-----------------------------------------------------------------------------------
void SquadronEncounter::Spawn()
{
    GameMode* gameMode = GameMode::GetCurrent();

    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(0.0f, 0.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(1.0f, 1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(1.0f, -1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(-1.0f, 1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(-1.0f, -1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(0.0f, 1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(0.0f, -1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(1.0f, 0.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(CalculateSpawnPosition(Vector2(-1.0f, 0.0f))));
}

