#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

class BattleRoyaleMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BattleRoyaleMinigameMode();
    virtual ~BattleRoyaleMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize();
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual Encounter* GetRandomMediumEncounter(const Vector2& center, float radius) override;
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};