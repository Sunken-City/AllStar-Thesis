#pragma once
#include "Game/GameModes/GameMode.hpp"

class InstancedGameMode : public GameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    InstancedGameMode();
    virtual ~InstancedGameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players) override;
    virtual void CleanUp() override;
    virtual void Update(float deltaSeconds) override;
    void AddGameModeInstance(GameMode* mode, PlayerShip* player);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    std::vector<GameMode*> m_gameModeInstances;
    std::map<GameMode*, PlayerShip*> m_playerGamemodeMap;
};
