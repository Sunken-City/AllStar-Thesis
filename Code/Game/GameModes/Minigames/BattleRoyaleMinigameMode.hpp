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
    virtual void Update(float deltaSeconds);
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};