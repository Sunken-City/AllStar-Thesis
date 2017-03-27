#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

class SuddenDeathMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    SuddenDeathMinigameMode();
    virtual ~SuddenDeathMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);

    virtual Encounter* GetRandomMediumEncounter(const Vector2& center, float radius) override;
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};
