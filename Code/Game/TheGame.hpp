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
class TextRenderable2D;
class WidgetBase;
class ParticleSystem;

//-----------------------------------------------------------------------------------
class TheGame
{
public:
    TheGame();
    ~TheGame();

    void ClearPlayers();

    void Update(float deltaTime);
    void Render() const;
    void InitializeGameOverState();

    static TheGame* instance;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static unsigned int const BACKGROUND_LAYER = 0;
    static unsigned int const BACKGROUND_STARS_LAYER_SLOWER = 100;
    static unsigned int const BACKGROUND_STARS_LAYER = 200;
    static unsigned int const BACKGROUND_PARTICLES_LAYER = 600;
    static unsigned int const BACKGROUND_PARTICLES_BLOOM_LAYER = 650;
    static unsigned int const GEOMETRY_LAYER = 700;
    static unsigned int const ITEM_LAYER = 800;
    static unsigned int const ITEM_TEXT_LAYER = 850;
    static unsigned int const ENEMY_LAYER = 900;
    static unsigned int const PLAYER_LAYER = 1000;
    static unsigned int const SHIELD_LAYER = 1050;
    static unsigned int const BULLET_LAYER_BLOOM = 1100;
    static unsigned int const BULLET_LAYER = 1200;
    static unsigned int const TEXT_PARTICLE_LAYER = 1300;
    static unsigned int const FOREGROUND_LAYER = 2000;
    static unsigned int const UI_LAYER = 3000;
    static unsigned int const TEXT_LAYER = 3500;
    static unsigned int const FULL_SCREEN_EFFECT_LAYER = 4000;
    static unsigned int const STAT_GRAPH_LAYER_BACKGROUND = 4400;
    static unsigned int const STAT_GRAPH_LAYER = 4500;
    static unsigned int const STAT_GRAPH_LAYER_TEXT = 4600;
    static unsigned int const FULL_SCREEN_EFFECT_OVERLAY_LAYER = 5000;

    static const float TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI;
    
private:
    TheGame& operator= (const TheGame& other) = delete;
    void InitializeKeyMappingsForPlayer(PlayerPilot* playerPilot);
    void RegisterSprites();
    void RegisterSpriteAnimations();
    void RegisterParticleEffects();
    void EnqueueMinigames();
    void InitializeSpriteLayers();

    void InitializeMainMenuState();
    void CleanupMainMenuState(unsigned int);
    void UpdateMainMenu(float deltaSeconds);
    void PressStart(NamedProperties& properties);
    void RenderMainMenu() const;

    void InitializePlayerJoinState();
    void CleanupPlayerJoinState(unsigned int);
    void UpdatePlayerJoin(float deltaSeconds);
    void RenderPlayerJoin() const;

    void InitializeAssemblyGetReadyState();
    void CleanupAssemblyGetReadyState(unsigned int);
    void UpdateAssemblyGetReady(float deltaSeconds);
    void RenderAssemblyGetReady() const;

    void InitializeAssemblyPlayingState();
    void CleanupAssemblyPlayingState(unsigned int);
    void UpdateAssemblyPlaying(float deltaSeconds);
    void RenderAssemblyPlaying() const;

    void InitializeAssemblyResultsState();
    void CleanupAssemblyResultsState(unsigned int);
    void UpdateAssemblyResults(float deltaSeconds);
    void RenderAssemblyResults() const;

    void InitializeMinigameGetReadyState();
    void CleanupMinigameGetReadyState(unsigned int);
    void UpdateMinigameGetReady(float deltaSeconds);
    void RenderMinigameGetReady() const;

    void InitializeMinigamePlayingState();
    void CleanupMinigamePlayingState(unsigned int);
    void UpdateMinigamePlaying(float deltaSeconds);

    void CheckForGamePaused();

    void RenderMinigamePlaying() const;

    void InitializeMinigameResultsState();
    void CleanupMinigameResultsState(unsigned int);
    void UpdateMinigameResults(float deltaSeconds);
    void RenderMinigameResults() const;

    void CleanupGameOverState(unsigned int);
    void UpdateGameOver(float deltaSeconds);
    void RenderGameOver() const;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    SoundID SFX_UI_ADVANCE;
    int m_numberOfMinigames = 3;
    int m_numberOfPlayers = 0;
    bool m_hasKeyboardPlayer = false;
    std::vector<PlayerPilot*> m_playerPilots;
    std::vector<PlayerShip*> m_players;
    std::queue<GameMode*> m_queuedMinigameModes;
    GameMode* m_currentGameMode;

private:
    Material* m_pauseFBOEffect = nullptr;
    Material* m_rainbowFBOEffect = nullptr;
    ParticleSystem* m_titleParticles = nullptr;
    Sprite* m_gameOverText = nullptr;
    Sprite* m_readyText[4];
    TextRenderable2D* m_titleText = nullptr;
    WidgetBase* m_gamePausedLabel = nullptr;
};
