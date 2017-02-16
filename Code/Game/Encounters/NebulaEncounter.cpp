#include "Game/Encounters/NebulaEncounter.hpp"
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

    gameMode->SpawnEntityInGameWorld(new Grunt(GetSpawnPosition(Vector2(0.0f, 0.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(GetSpawnPosition(Vector2(1.0f, 1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(GetSpawnPosition(Vector2(1.0f, -1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(GetSpawnPosition(Vector2(-1.0f, 1.0f))));
    gameMode->SpawnEntityInGameWorld(new Grunt(GetSpawnPosition(Vector2(-1.0f, -1.0f))));
}

