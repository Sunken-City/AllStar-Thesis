#pragma once

#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputMap.hpp"
#include "GameModes/GameMode.hpp"
#include <queue>

class Entity;
class PlayerShip;
class PlayerPilot;
class Ship;
class Item;
class GameMode;
class NamedProperties;

//-----------------------------------------------------------------------------------
class TheGame
{
public:
    TheGame();
    ~TheGame();
    void Update(float deltaTime);
    void Render() const;
    void InitializeGameOverState();

    static TheGame* instance;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static unsigned int const BACKGROUND_LAYER = 0;
    static unsigned int const GEOMETRY_LAYER = 7;
    static unsigned int const ITEM_LAYER = 8;
    static unsigned int const ENEMY_LAYER = 9;
    static unsigned int const PLAYER_LAYER = 10;
    static unsigned int const PLAYER_BULLET_LAYER = 11;
    static unsigned int const ENEMY_BULLET_LAYER = 12;
    static unsigned int const FOREGROUND_LAYER = 20;
    static unsigned int const UI_LAYER = 30;
    
private:
    TheGame& operator= (const TheGame& other) = delete;
    void InitializeKeyMappingsForPlayer(PlayerPilot* playerPilot);
    void RegisterSprites();

    void CleanupGameOverState(unsigned int);
    void UpdateGameOver(float deltaSeconds);
    void RenderGameOver() const;

    void InitializeAssemblyState();
    void CleanupAssemblyState(unsigned int);
    void UpdateAssemblyPlaying(float deltaSeconds);
    void RenderAssemblyPlaying() const;

    void InitializeAssemblyResultsState();
    void CleanupAssemblyResultsState(unsigned int);
    void UpdateAssemblyResults(float deltaSeconds);
    void RenderAssemblyResults() const;

    void InitializeMinigameState();
    void CleanupMinigameState(unsigned int);
    void UpdateMinigamePlaying(float deltaSeconds);
    void RenderMinigamePlaying() const;

    void InitializeMinigameResultsState();
    void CleanupMinigameResultsState(unsigned int);
    void UpdateMinigameResults(float deltaSeconds);
    void RenderMinigameResults() const;

    void InitializeMainMenuState();
    void CleanupMainMenuState(unsigned int);
    void UpdateMainMenu(float deltaSeconds);
    void RenderMainMenu() const;
    void EnqueueMinigames();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    SoundID SFX_UI_ADVANCE;
    int m_numberOfMinigames = 3;
    std::vector<PlayerPilot*> m_playerPilots;
    std::vector<PlayerShip*> m_players;
    std::queue<GameMode*> m_queuedMinigameModes;
    GameMode* m_currentGameMode;
};
