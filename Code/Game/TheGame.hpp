#pragma once

#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputMap.hpp"
#include "GameModes/GameMode.hpp"

class Entity;
class PlayerShip;
class PlayerPilot;
class Ship;
class Item;
class GameMode;

//-----------------------------------------------------------------------------------
class TheGame
{
public:
    TheGame();
    ~TheGame();
    void Update(float deltaTime);
    void Render() const;
    void SpawnBullet(Ship* creator);
    void SpawnPickup(Item* item, const Vector2& spawnPosition);
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
    void CleanupGameOverState(unsigned int);
    void UpdateGameOver(float deltaSeconds);
    void RenderGameOver() const;
    void RegisterSprites();
    void InitializePlayingState();
    void UpdatePlaying(float deltaSeconds);
    void RenderPlaying() const;
    void CleanupPlayingState(unsigned int);

    void InitializeMainMenuState();
    void CleanupMainMenuState(unsigned int);
    void UpdateMainMenu(float deltaSeconds);
    void RenderMainMenu() const;
    void InitializeKeyMappingsForPlayer(PlayerPilot* playerPilot);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<PlayerPilot*> m_playerPilots;
    GameMode* m_allGameModes[4];
    GameMode* m_currentGameMode;
};
