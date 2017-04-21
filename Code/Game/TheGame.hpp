#pragma once

#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputMap.hpp"
#include "GameModes/GameMode.hpp"
#include <queue>
#include <set>
#include "Engine/Time/Time.hpp"

class Entity;
class PlayerShip;
class PlayerPilot;
class Ship;
class Item;
class GameMode;
class NamedProperties;
class TextRenderable2D;
class WidgetBase;
class ShaderProgram;
class ParticleSystem;
class BarGraphRenderable2D;
class LabelWidget;

//-----------------------------------------------------------------------------------
class TheGame
{
public:
    TheGame();
    ~TheGame();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    void ClearPlayers();
    void Update(float deltaTime);
    void Render() const;
    void InitializeGameOverState();

    static TheGame* instance;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static unsigned int const BACKGROUND_LAYER = 0;
    static unsigned int const BACKGROUND_STARS_LAYER_SLOWER = 100;
    static unsigned int const BACKGROUND_STARS_LAYER = 200;
    static unsigned int const BACKGROUND_GEOMETRY_LAYER = 500;
    static unsigned int const BACKGROUND_GEOMETRY_LAYER_ABOVE = 501;
    static unsigned int const BACKGROUND_PARTICLES_LAYER = 600;
    static unsigned int const BACKGROUND_PARTICLES_BLOOM_LAYER = 650;
    static unsigned int const GEOMETRY_LAYER = 700;
    static unsigned int const CRATE_LAYER = 730;
    static unsigned int const CRATE_DECORATION_LAYER = 740;
    static unsigned int const POWER_UP_LAYER = 800;
    static unsigned int const EQUIP_LAYER = 810;
    static unsigned int const ENEMY_LAYER = 900;
    static unsigned int const PLAYER_LAYER = 1000;
    static unsigned int const SHIELD_LAYER = 1050;
    static unsigned int const BULLET_LAYER_BLOOM = 1100;
    static unsigned int const BULLET_LAYER = 1200;
    static unsigned int const FOREGROUND_LAYER = 2000;
    static unsigned int const TEXT_PARTICLE_LAYER = 2300;
    static unsigned int const ITEM_TEXT_LAYER = 2400;
    static unsigned int const BACKGROUND_UI_LAYER = 2500;
    static unsigned int const UI_LAYER = 3000;
    static unsigned int const TEXT_LAYER = 3500;
    static unsigned int const FULL_SCREEN_EFFECT_LAYER = 4000;
    static unsigned int const STAT_GRAPH_LAYER_BACKGROUND = 4400;
    static unsigned int const STAT_GRAPH_LAYER = 4500;
    static unsigned int const STAT_GRAPH_LAYER_TEXT = 4600;
    static unsigned int const FULL_SCREEN_EFFECT_OVERLAY_LAYER = 5000;
    static unsigned int const FBO_FREE_TEXT_LAYER = 6000;

    static const float TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI;
    static const float TOTAL_TRANSITION_TIME_SECONDS;
    static const float TRANSITION_TIME_SECONDS;
    
private:
    TheGame& operator= (const TheGame& other) = delete;
    void InitializeKeyMappingsForPlayer(PlayerPilot* playerPilot);
    void PreloadAudio();
    void RegisterSprites();
    void RegisterSpriteAnimations();
    void RegisterParticleEffects();
    void EnqueueMinigames();
    GameMode* GetRandomUniqueGameMode();
    void InitializeSpriteLayers();
    void CheckForGamePaused();
    bool IsThereTieForFirst();
    int GetMaxScore();
    std::vector<PlayerShip*> GetTiedWinners();

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

    void RenderSplitscreenLines() const;

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
    void RenderMinigamePlaying() const;

    void InitializeMinigameResultsState();
    void CleanupMinigameResultsState(unsigned int);
    void UpdateMinigameResults(float deltaSeconds);
    void RenderMinigameResults() const;

    void CleanupGameOverState(unsigned int);
    void UpdateGameOver(float deltaSeconds);
    void RenderGameOver() const;
    void RenderDebug() const;
public:
    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const int MAX_NUM_PLAYERS = 4;
    static const char* NO_CONTROLLER_STRING;
    static const char* PRESS_START_TO_JOIN_STRING;
    static const char* PRESS_START_TO_READY_STRING;
    static const char* READY_STRING;
    static constexpr float POWER_UP_DURATION = 5.0f;
    static constexpr float SHIELD_ACTIVE_DURATION = 10.0f;
    static constexpr float BOOST_DURATION = 1.0f;
    static constexpr float DEATH_ANIMATION_LENGTH = 1.5f;
    static constexpr float WARP_ANIMATION_LENGTH = 1.5f;
    static constexpr float POWER_UP_PICKUP_ANIMATION_LENGTH = 0.15f;
    static constexpr float MUZZLE_FLASH_ANIMATION_LENGTH = 0.01f;
    static constexpr float CRATE_DESTRUCTION_ANIMATION_LENGTH = 0.6f;
    static constexpr float COLLISION_ANIMATION_LENGTH = 0.3f;
    static constexpr int POINTS_PER_PLACE[4] = { 7, 4, 2, 1 };
    static constexpr int MAX_POINTS = POINTS_PER_PLACE[0] * 3;
    static constexpr float GAME_OVER_ANIMATION_LENGTH = 5.0f;

    //AUDIO CONSTANTS/////////////////////////////////////////////////////////////////////
    static constexpr float BULLET_VOLUME = 0.5f;
    static constexpr float HIT_SOUND_VOLUME = 0.6f;
    static constexpr float PLAYER_HIT_SOUND_VOLUME = 0.8f;
    static constexpr float MUSIC_VOLUME = 0.3f;
    static constexpr float TELEPORT_VOLUME = 0.7f;

    typedef int GLint;
    typedef unsigned int GLuint;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    std::vector<PlayerPilot*> m_playerPilots;
    std::vector<PlayerShip*> m_players;
    std::queue<GameMode*> m_queuedMinigameModes;
    GameMode* m_currentGameMode;
    Material* m_UIMaterial = nullptr;
    ShaderProgram* m_UIShader = nullptr;
    SoundID SFX_UI_ADVANCE;
    SoundID m_menuMusic;
    SoundID m_resultsMusic;
    GLint m_bindingPoint;
    GLuint m_vortexUniformBuffer;
    unsigned int m_gamemodeFlags = 0;
    int m_numberOfMinigames = 3;
    int m_numberOfPlayers = 0;
    int m_numberOfReadyPlayers = 0;
    bool m_hasKeyboardPlayer = false;
    bool m_useFixedMinigames = false;

private:
    Sprite* m_shipPreviews[4];
    Sprite* m_rightArrows[4];
    Sprite* m_leftArrows[4];
    TextRenderable2D* m_rankText[4];
    TextRenderable2D* m_scoreEarnedText[4];
    TextRenderable2D* m_totalScoreText[4];
    TextRenderable2D* m_joinText[4];
    TextRenderable2D* m_readyText[4];
    WidgetBase* m_fpsCounter = nullptr;
    TextRenderable2D* m_titleText = nullptr;
    TextRenderable2D* m_winnerText = nullptr;
    BarGraphRenderable2D** m_playerRankPodiums = nullptr;
    PlayerShip* m_winner = nullptr;
    Sprite* m_background = nullptr;
    Material* m_transitionFBOEffect = nullptr;
    Material* m_pauseFBOEffect = nullptr;
    Material* m_resultsBackgroundEffect = nullptr;
    Material* m_rainbowFBOEffect = nullptr;
    ParticleSystem* m_titleParticles = nullptr;
    ParticleSystem* m_confettiParticles = nullptr;
    unsigned int m_paletteOffsets[4];
};
