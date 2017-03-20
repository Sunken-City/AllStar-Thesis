#include "Game/TheGame.hpp"
#include "Game/StateMachine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/Console.hpp"
#include "Engine/Input/Logging.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixStack4x4.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/TextRendering/TextBox.hpp"
#include "Engine/Fonts/BitmapFont.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Ship.hpp"
#include "Entities/Props/ItemCrate.hpp"
#include "Entities/Grunt.hpp"
#include "Entities/Pickup.hpp"
#include "Engine/Input/InputDevices/KeyboardInputDevice.hpp"
#include "Engine/Input/InputDevices/MouseInputDevice.hpp"
#include "Pilots/PlayerPilot.hpp"
#include "Engine/Input/InputDevices/XInputDevice.hpp"
#include "Engine/Input/InputValues.hpp"
#include "GameModes/AssemblyMode.hpp"
#include "GameModes/Minigames/BattleRoyaleMinigameMode.hpp"
#include "Engine/Input/InputOutputUtils.hpp"
#include "Engine/Core/Events/EventSystem.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "GameCommon.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Game/Entities/TextSplash.hpp"
#include "GameModes/Minigames/DeathBattleMinigameMode.hpp"
#include "GameModes/InstancedGameMode.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <gl/GL.h>
#include "Engine/Renderer/2D/BarGraphRenderable2D.hpp"

TheGame* TheGame::instance = nullptr;

const float TheGame::TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI = 0.5f;
const float TheGame::TOTAL_TRANSITION_TIME_SECONDS = 0.5f;
const float TheGame::TRANSITION_TIME_SECONDS = TOTAL_TRANSITION_TIME_SECONDS * 0.5f;

const char* TheGame::NO_CONTROLLER_STRING = "No Controller Connected :c";
const char* TheGame::PRESS_START_TO_JOIN_STRING = "Press Start to Join";
const char* TheGame::PRESS_START_TO_READY_STRING = "Press Start when Ready";
const char* TheGame::READY_STRING = "Ready!";

//-----------------------------------------------------------------------------------
TheGame::TheGame()
    : m_currentGameMode(nullptr)
    , SFX_UI_ADVANCE(AudioSystem::instance->CreateOrGetSound("Data/SFX/UI/UI_Select_01.wav"))
    , m_menuMusic(AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Function - 02 Acylite.ogg"))
    , m_resultsMusic(AudioSystem::instance->CreateOrGetSound("Data/Music/Overcast.ogg"))
{
    srand(GetTimeBasedSeed());
    ResourceDatabase::instance = new ResourceDatabase();
    RegisterSprites();
    RegisterSpriteAnimations();
    RegisterParticleEffects();
    PreloadAudio();
    EventSystem::RegisterObjectForEvent("StartGame", this, &TheGame::PressStart);
    InitializeSpriteLayers();

    m_transitionFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\transitionShader.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );
    m_pauseFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\pixelationWaves.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );
    m_pauseFBOEffect->SetFloatUniform("gPixelationFactor", 16.0f);
    m_rainbowFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\rainbow.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );

    m_transitionFBOEffect->SetFloatUniform("gEffectTime", -10.0f);
    m_transitionFBOEffect->SetFloatUniform("gEffectDurationSeconds", TRANSITION_TIME_SECONDS);
    m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
    SpriteGameRenderer::instance->AddEffectToLayer(m_transitionFBOEffect, FULL_SCREEN_EFFECT_LAYER);
    SetGameState(GameState::MAIN_MENU);
    InitializeMainMenuState();
}


//-----------------------------------------------------------------------------------
TheGame::~TheGame()
{
    SetGameState(GameState::SHUTDOWN);
    TextSplash::Cleanup();

    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_transitionFBOEffect, FULL_SCREEN_EFFECT_LAYER);
    delete m_transitionFBOEffect->m_shaderProgram;
    delete m_transitionFBOEffect;
    delete m_pauseFBOEffect->m_shaderProgram;
    delete m_pauseFBOEffect;
    delete m_rainbowFBOEffect->m_shaderProgram;
    delete m_rainbowFBOEffect;

    if (m_currentGameMode)
    {
        m_currentGameMode->CleanUp();
        delete m_currentGameMode;
    }

    unsigned int numModes = m_queuedMinigameModes.size();
    for (unsigned int i = 0; i < numModes; ++i)
    {
        GameMode* mode = m_queuedMinigameModes.front();
        delete mode;
        m_queuedMinigameModes.pop();
    }
    ClearPlayers();

    delete ResourceDatabase::instance;
    ResourceDatabase::instance = nullptr;
}

//-----------------------------------------------------------------------------------
void TheGame::ClearPlayers()
{
    m_numberOfPlayers = 0;
    m_hasKeyboardPlayer = false;
    for (PlayerPilot* pilot : m_playerPilots)
    {
        delete pilot;
    }
    m_playerPilots.clear();
    for (PlayerShip* ship : m_players)
    {
        delete ship;
    }
    m_players.clear();

    InputSystem::instance->ClearAndRecreateInputDevices();
}

//-----------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds)
{
    g_secondsInState += deltaSeconds;
    SpriteGameRenderer::instance->Update(deltaSeconds);
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::TILDE))
    {
        Console::instance->ToggleConsole();
    }
    if (Console::instance->IsActive())
    {
        return;
    }
    DispatchRunAfterSeconds();

    switch (GetGameState())
    {
    case MAIN_MENU:
        UpdateMainMenu(deltaSeconds);
        break;
    case PLAYER_JOIN:
        UpdatePlayerJoin(deltaSeconds);
        break;
    case ASSEMBLY_GET_READY:
        UpdateAssemblyGetReady(deltaSeconds);
        break;
    case ASSEMBLY_PLAYING:
        UpdateAssemblyPlaying(deltaSeconds);
        break;
    case ASSEMBLY_RESULTS:
        UpdateAssemblyResults(deltaSeconds);
        break;
    case GAME_RESULTS_SCREEN:
        UpdateGameOver(deltaSeconds);
        break;
    case MINIGAME_GET_READY:
        UpdateMinigameGetReady(deltaSeconds);
        break;
    case MINIGAME_PLAYING:
        UpdateMinigamePlaying(deltaSeconds);
        break;
    case MINIGAME_RESULTS:
        UpdateMinigameResults(deltaSeconds);
        break;
    default:
        break;
    }
    TextSplash::Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void TheGame::Render() const
{
    ENSURE_NO_MATRIX_STACK_SIDE_EFFECTS(Renderer::instance->m_viewStack);
    ENSURE_NO_MATRIX_STACK_SIDE_EFFECTS(Renderer::instance->m_projStack);
    if (Console::instance->IsActive())
    {
        Console::instance->Render();
    }
    switch (GetGameState())
    {
    case MAIN_MENU:
        RenderMainMenu();
        break;
    case PLAYER_JOIN:
        RenderPlayerJoin();
        break;
    case ASSEMBLY_GET_READY:
        RenderAssemblyGetReady();
        break;
    case ASSEMBLY_PLAYING:
        RenderAssemblyPlaying();
        break;
    case ASSEMBLY_RESULTS:
        RenderAssemblyResults();
        break;
    case GAME_RESULTS_SCREEN:
        RenderGameOver();
        break;
    case MINIGAME_GET_READY:
        RenderMinigameGetReady();
        break;
    case MINIGAME_PLAYING:
        RenderMinigamePlaying();
        break;
    case MINIGAME_RESULTS:
        RenderMinigameResults();
        break;
    default:
        break;

    }

}

//-----------------------------------------------------------------------------------
//MAIN MENU/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMainMenuState()
{
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
    m_titleText = new TextRenderable2D("ALLSTAR", Transform2D(Vector2(0.0f, 0.0f)), TEXT_LAYER);
    SpriteGameRenderer::instance->AddEffectToLayer(m_rainbowFBOEffect, BACKGROUND_PARTICLES_BLOOM_LAYER);
    m_titleParticles = new ParticleSystem("Title", BACKGROUND_PARTICLES_BLOOM_LAYER, Vector2(0.0f, -15.0f));
    if (!g_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_menuMusic, 0.6f);
    }
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMainMenuState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMainMenuState(unsigned int)
{
    delete m_titleText;
    ParticleSystem::DestroyImmediately(m_titleParticles);
    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_rainbowFBOEffect, BACKGROUND_PARTICLES_BLOOM_LAYER);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMainMenu(float)
{
    m_titleText->m_transform.SetRotationDegrees(m_titleText->m_transform.GetWorldRotationDegrees() + 2.0f);
    m_titleText->m_transform.SetScale(Vector2(fabs(sin(g_secondsInState * 2.0f)) + 0.5f));

    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        NamedProperties properties;
        PressStart(properties);
    }
}

//-----------------------------------------------------------------------------------
void TheGame::PressStart(NamedProperties&)
{
    if (IsTransitioningStates())
    {
        return;
    }

    RunAfterSeconds([]()
    {
        SetGameState(PLAYER_JOIN);
        TheGame::instance->InitializePlayerJoinState();
        //TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
    }, TRANSITION_TIME_SECONDS);

    BeginTransitioning();
    m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelStarWipe")->m_texture);
    m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
    m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMainMenu() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::VERY_GRAY);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
void TheGame::EnqueueMinigames()
{
//     for (int i = 0; i < m_numberOfMinigames; ++i)
//     {
//         InstancedGameMode* mode = new InstancedGameMode();
//         for (PlayerShip* player : m_players)
//         {
//             mode->AddGameModeInstance(new BattleRoyaleMinigameMode(), player);
//         }
//         m_queuedMinigameModes.push(mode);
//     }
    m_queuedMinigameModes.push(new DeathBattleMinigameMode());
    m_queuedMinigameModes.push(new BattleRoyaleMinigameMode());
    m_queuedMinigameModes.push(new DeathBattleMinigameMode());
}

//-----------------------------------------------------------------------------------
//PLAYER JOIN/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializePlayerJoinState()
{
    ClearPlayers();
    //m_titleText = new TextRenderable2D("ALLSTAR", Transform2D(Vector2(0.0f, 0.0f)), TEXT_LAYER);
    for (int i = 0; i < MAX_NUM_PLAYERS; ++i)
    {
        m_joinText[i] = new TextRenderable2D(NO_CONTROLLER_STRING, Transform2D(), TheGame::TEXT_LAYER);
        m_joinText[i]->m_fontSize = 0.25f;
        m_joinText[i]->m_color = RGBA::GBWHITE;
        m_readyText[i] = new TextRenderable2D(PRESS_START_TO_READY_STRING, Transform2D(), TheGame::TEXT_LAYER);
        m_readyText[i]->m_fontSize = 0.3f;
        m_readyText[i]->m_color = RGBA::CORNFLOWER_BLUE;
        m_readyText[i]->Disable();

        m_paletteOffsets[i] = i;
        m_shipPreviews[i] = new Sprite("DefaultChassis", TheGame::PLAYER_LAYER);
        m_shipPreviews[i]->m_transform.SetScale(Vector2(5.0f));
        m_shipPreviews[i]->m_material = new Material(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/paletteSwap2D.frag"), SpriteGameRenderer::instance->m_defaultRenderState);
        m_shipPreviews[i]->m_material->ReplaceSampler(Renderer::instance->CreateSampler(GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP));
        m_shipPreviews[i]->m_material->SetFloatUniform("PaletteOffset", static_cast<float>(i) / 16.0f);
        m_shipPreviews[i]->m_material->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ShipColorPalettes")->m_texture);
        m_shipPreviews[i]->Disable();

        m_leftArrows[i] = new Sprite("Arrow", TheGame::PLAYER_LAYER);
        m_leftArrows[i]->m_transform.SetScale(Vector2(2.0f));
        m_leftArrows[i]->m_material = new Material(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/paletteSwap2D.frag"), SpriteGameRenderer::instance->m_defaultRenderState);
        m_leftArrows[i]->m_material->ReplaceSampler(Renderer::instance->CreateSampler(GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP));
        m_leftArrows[i]->m_material->SetFloatUniform("PaletteOffset", static_cast<float>(Mod(i + 1, 16)) / 16.0f);
        m_leftArrows[i]->m_material->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ShipColorPalettes")->m_texture);
        m_leftArrows[i]->Disable();
        m_rightArrows[i] = new Sprite("Arrow", TheGame::PLAYER_LAYER);
        m_rightArrows[i]->m_transform.SetScale(Vector2(2.0f));
        m_rightArrows[i]->m_material = new Material(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/paletteSwap2D.frag"), SpriteGameRenderer::instance->m_defaultRenderState);
        m_rightArrows[i]->m_material->ReplaceSampler(Renderer::instance->CreateSampler(GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP));
        m_rightArrows[i]->m_material->SetFloatUniform("PaletteOffset", static_cast<float>(Mod(i - 1, 16)) / 16.0f);
        m_rightArrows[i]->m_transform.SetRotationDegrees(180.0f);
        m_rightArrows[i]->m_material->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ShipColorPalettes")->m_texture);
        m_rightArrows[i]->Disable();
    }
    m_readyText[0]->m_transform.SetPosition(Vector2(-5.0f, 1.0f));
    m_readyText[1]->m_transform.SetPosition(Vector2(5.0f, 1.0f));
    m_readyText[2]->m_transform.SetPosition(Vector2(-5.0f, -4.5f));
    m_readyText[3]->m_transform.SetPosition(Vector2(5.0f, -4.5f));

    m_joinText[0]->m_transform.SetPosition(Vector2(-5.0f, 3.0f));
    m_joinText[1]->m_transform.SetPosition(Vector2(5.0f, 3.0f));
    m_joinText[2]->m_transform.SetPosition(Vector2(-5.0f, -3.0f));
    m_joinText[3]->m_transform.SetPosition(Vector2(5.0f, -3.0f));

    m_shipPreviews[0]->m_transform.SetPosition(Vector2(-5.0f, 3.0f));
    m_shipPreviews[1]->m_transform.SetPosition(Vector2(5.0f, 3.0f));
    m_shipPreviews[2]->m_transform.SetPosition(Vector2(-5.0f, -3.0f));
    m_shipPreviews[3]->m_transform.SetPosition(Vector2(5.0f, -3.0f));

    m_leftArrows[0]->m_transform.SetPosition(Vector2(-3.0f, 3.0f));
    m_leftArrows[1]->m_transform.SetPosition(Vector2(7.0f, 3.0f));
    m_leftArrows[2]->m_transform.SetPosition(Vector2(-3.0f, -3.0f));
    m_leftArrows[3]->m_transform.SetPosition(Vector2(7.0f, -3.0f));

    m_rightArrows[0]->m_transform.SetPosition(Vector2(-7.0f, 3.0f));
    m_rightArrows[1]->m_transform.SetPosition(Vector2(3.0f, 3.0f));
    m_rightArrows[2]->m_transform.SetPosition(Vector2(-7.0f, -3.0f));
    m_rightArrows[3]->m_transform.SetPosition(Vector2(3.0f, -3.0f));

    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupPlayerJoinState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupPlayerJoinState(unsigned int)
{
    for (int i = 0; i < MAX_NUM_PLAYERS; ++i)
    {
        delete m_readyText[i];
        m_readyText[i] = nullptr;
        delete m_joinText[i];
        m_joinText[i] = nullptr;

        delete m_shipPreviews[i]->m_material->m_shaderProgram;
        delete m_shipPreviews[i]->m_material;
        delete m_shipPreviews[i];
        m_shipPreviews[i] = nullptr;
        delete m_leftArrows[i]->m_material->m_shaderProgram;
        delete m_leftArrows[i]->m_material;
        delete m_leftArrows[i];
        m_leftArrows[i] = nullptr;
        delete m_rightArrows[i]->m_material->m_shaderProgram;
        delete m_rightArrows[i]->m_material;
        delete m_rightArrows[i];
        m_rightArrows[i] = nullptr;
    }

    for (unsigned int i = 0; i < m_playerPilots.size(); ++i)
    {
        PlayerShip* player = new PlayerShip(TheGame::instance->m_playerPilots[i]);
        player->HideUI();
        TheGame::instance->m_players.push_back(player);

        float paletteIndex = static_cast<float>(m_paletteOffsets[i]) / 16.0f;
        m_players[i]->m_sprite->m_material->SetFloatUniform("PaletteOffset", paletteIndex);
    }

    AudioSystem::instance->StopSound(m_menuMusic);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlayerJoin(float)
{
    if (IsTransitioningStates())
    {
        return;
    }

    for (int i = 0; i < MAX_NUM_PLAYERS; ++i)
    {
    }

    //If someone presses their button a second time, we know all players are in and we're ready to start.
    for (unsigned int i = 0; i < m_playerPilots.size(); ++i)
    {
        PlayerPilot* pilot = m_playerPilots[i];

        if(pilot->m_inputMap.WasJustPressed("Accept") && (m_numberOfReadyPlayers < m_numberOfPlayers) && m_readyText[i]->m_text == PRESS_START_TO_READY_STRING)
        {
            ++m_numberOfReadyPlayers;
            m_readyText[i]->m_text = READY_STRING;
            m_readyText[i]->m_color = RGBA::GREEN;
        }
        else if (pilot->m_inputMap.WasJustPressed("Back") && (m_numberOfReadyPlayers > 0) && m_readyText[i]->m_text == READY_STRING)
        {
            --m_numberOfReadyPlayers;
            m_readyText[i]->m_text = PRESS_START_TO_READY_STRING;
            m_readyText[i]->m_color = RGBA::CORNFLOWER_BLUE;
        }
        else if (((pilot->m_inputMap.WasJustPressed("Accept")) && (m_numberOfReadyPlayers == m_numberOfPlayers)) || (m_numberOfPlayers == 4 && InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9)))
        {
            RunAfterSeconds([]()
            {
                SetGameState(ASSEMBLY_GET_READY);
                TheGame::instance->InitializeAssemblyGetReadyState();
            }, TRANSITION_TIME_SECONDS);

            BeginTransitioning();
            m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelStarWipe")->m_texture);
            m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
            m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
            AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
            return;
        }
        else if (pilot->m_inputMap.WasJustPressed("CycleColorsLeft"))
        {
            m_paletteOffsets[i] = Mod((m_paletteOffsets[i] - 1), 16);
            float paletteIndex = static_cast<float>(m_paletteOffsets[i]) / 16.0f;
            m_shipPreviews[i]->m_material->SetFloatUniform("PaletteOffset", paletteIndex);

            float rightPaletteIndex = static_cast<float>(Mod((m_paletteOffsets[i] - 1), 16)) / 16.0f;
            float leftPaletteIndex = static_cast<float>(Mod((m_paletteOffsets[i] + 1), 16)) / 16.0f;
            m_leftArrows[i]->m_material->SetFloatUniform("PaletteOffset", leftPaletteIndex);
            m_rightArrows[i]->m_material->SetFloatUniform("PaletteOffset", rightPaletteIndex);
        }
        else if (pilot->m_inputMap.WasJustPressed("CycleColorsRight"))
        {
            m_paletteOffsets[i] = Mod((m_paletteOffsets[i] + 1), 16);
            float paletteIndex = static_cast<float>(m_paletteOffsets[i]) / 16.0f;
            m_shipPreviews[i]->m_material->SetFloatUniform("PaletteOffset", paletteIndex);

            float rightPaletteIndex = static_cast<float>(Mod((m_paletteOffsets[i] - 1), 16)) / 16.0f;
            float leftPaletteIndex = static_cast<float>(Mod((m_paletteOffsets[i] + 1), 16)) / 16.0f;
            m_leftArrows[i]->m_material->SetFloatUniform("PaletteOffset", leftPaletteIndex);
            m_rightArrows[i]->m_material->SetFloatUniform("PaletteOffset", rightPaletteIndex);
        }

        static const float DEADZONE_BEFORE_ROTATION = 0.3f;
        static const float DEADZONE_BEFORE_ROTATION_SQUARED = DEADZONE_BEFORE_ROTATION * DEADZONE_BEFORE_ROTATION;
        Vector2 shootDirection = pilot->m_inputMap.GetVector2("ShootRight", "ShootUp");
        if (shootDirection.CalculateMagnitudeSquared() > DEADZONE_BEFORE_ROTATION_SQUARED)
        {
            m_shipPreviews[i]->m_transform.SetRotationDegrees(shootDirection.GetDirectionDegreesFromNormalizedVector());
        }
        else
        {
            float rotationAmount = MathUtils::Lerp(0.1f, m_shipPreviews[i]->m_transform.GetLocalRotationDegrees(), (float)GetCurrentTimeSeconds() * 50.0f);
            m_shipPreviews[i]->m_transform.SetRotationDegrees(rotationAmount);
        }
        m_rightArrows[i]->m_transform.SetScale(Vector2(2.0f + (sin(GetCurrentTimeSeconds()) * 0.5f)));
        m_leftArrows[i]->m_transform.SetScale(Vector2(2.0f + (sin(GetCurrentTimeSeconds()) * 0.5f)));
    }

    if (!m_hasKeyboardPlayer && m_numberOfPlayers < 4 && (InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9)))
    {
        //Hack to assert that we don't let a player double add themselves
        for (PlayerPilot* pilot : m_playerPilots)
        {
            if (pilot->m_controllerIndex == -1 && !InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
            {
                return;
            }
        }
        m_readyText[m_numberOfPlayers]->Enable();
        m_shipPreviews[m_numberOfPlayers]->Enable();
        m_leftArrows[m_numberOfPlayers]->Enable();
        m_rightArrows[m_numberOfPlayers]->Enable();
        m_joinText[m_numberOfPlayers]->Disable();
        PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
        pilot->m_controllerIndex = -1;
        m_playerPilots.push_back(pilot);
        InitializeKeyMappingsForPlayer(pilot);
    }

    for (unsigned int i = 0; i < 4; ++i)
    {
        XInputController* controller = InputSystem::instance->m_controllers[i];
        if (controller->IsConnected())
        {
            //Hack to assert that we don't let a player double add themselves
            bool alreadyAssignedToPlayer = false;
            for (PlayerPilot* pilot : m_playerPilots)
            {
                if (pilot->m_controllerIndex == i)
                {
                    alreadyAssignedToPlayer = true;
                    break;
                }
            }

            m_joinText[i]->m_text = PRESS_START_TO_JOIN_STRING;
            m_joinText[i]->m_color = RGBA::YELLOW;

            if ((!alreadyAssignedToPlayer) && (m_numberOfPlayers < 4) && controller->JustPressed(XboxButton::START))
            {
                m_readyText[m_numberOfPlayers]->Enable();
                m_shipPreviews[m_numberOfPlayers]->Enable();
                m_leftArrows[m_numberOfPlayers]->Enable();
                m_rightArrows[m_numberOfPlayers]->Enable();
                m_joinText[m_numberOfPlayers]->Disable();
                PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
                pilot->m_controllerIndex = i;
                m_playerPilots.push_back(pilot);
                InitializeKeyMappingsForPlayer(pilot);
            }
        }
        else
        {
            m_joinText[i]->m_text = NO_CONTROLLER_STRING;
            m_joinText[i]->m_color = RGBA::GBWHITE;
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderPlayerJoin() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBBLACK);
    SpriteGameRenderer::instance->Render();

    static const float LINE_WIDTH = 5.0f;
    Renderer::instance->BeginOrtho(SpriteGameRenderer::instance->m_windowVirtualWidth, SpriteGameRenderer::instance->m_windowVirtualHeight, Vector2::ZERO);
    {
        SpriteGameRenderer::instance->DrawLine(Vector2(0.0f, 20.0f), Vector2(0.0f, -20.0f), RGBA::WHITE, LINE_WIDTH);
        SpriteGameRenderer::instance->DrawLine(Vector2(20.0f, 0.0f), Vector2(-20.0f, 0.0f), RGBA::WHITE, LINE_WIDTH);
    }
    Renderer::instance->EndOrtho();
}

//-----------------------------------------------------------------------------------
//ASSEMBLY GET READY/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyGetReadyState()
{
    m_currentGameMode = static_cast<GameMode*>(new AssemblyMode());
    m_currentGameMode->InitializeReadyAnim();

    SpriteGameRenderer::instance->DisableAllLayers();
    SpriteGameRenderer::instance->EnableLayer(TEXT_LAYER);
    SpriteGameRenderer::instance->EnableLayer(UI_LAYER);
    SpriteGameRenderer::instance->EnableLayer(FULL_SCREEN_EFFECT_LAYER);

    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyGetReadyState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyGetReadyState(unsigned int)
{
    SpriteGameRenderer::instance->EnableAllLayers();
    m_currentGameMode->CleanupReadyAnim();
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyGetReady(float deltaSeconds)
{
    if (IsTransitioningStates())
    {
        return;
    }
    m_currentGameMode->UpdateReadyAnim(deltaSeconds);

    if (g_secondsInState < TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI)
    {
        return;
    }
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept") || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
        {
            RunAfterSeconds([]()
            {
                SetGameState(ASSEMBLY_PLAYING);
                TheGame::instance->InitializeAssemblyPlayingState();
                TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
            }, TRANSITION_TIME_SECONDS);

            BeginTransitioning();
            m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
            m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
            m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
            AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
            return;
        }

    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyGetReady() const
{
    SpriteGameRenderer::instance->SetClearColor(m_currentGameMode->m_readyBGColor);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//ASSEMBLY PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyPlayingState()
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->ShowUI();
    }

//     m_gamePausedLabel = UISystem::instance->CreateWidget("Label");
//     m_gamePausedLabel->SetProperty<std::string>("Name", "GamePausedLabel");
//     m_gamePausedLabel->SetProperty<std::string>("Text", "Game Paused");
//     m_gamePausedLabel->SetProperty("BackgroundColor", RGBA::CLEAR);
//     m_gamePausedLabel->SetProperty("BorderWidth", 0.0f);
//     m_gamePausedLabel->SetProperty("TextSize", 7.0f);
//     m_gamePausedLabel->SetProperty("Offset", Vector2(1600.0f/4.0f, 900.0f/2.0f));
//     UISystem::instance->AddWidget(m_gamePausedLabel);
//     m_gamePausedLabel->SetHidden();

    m_currentGameMode->Initialize(m_players);
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());

    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyPlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyPlayingState(unsigned int)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->HideUI();
    }
    //UISystem::instance->DeleteWidget(m_gamePausedLabel);
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyPlaying(float deltaSeconds)
{
    CheckForGamePaused();
    if (g_isGamePaused)
    {
        return;
    }

    m_currentGameMode->Update(deltaSeconds);
    if (!m_currentGameMode->m_isPlaying || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
    {
        if (IsTransitioningStates())
        {
            return;
        }

        RunAfterSeconds([]()
        {
            SetGameState(ASSEMBLY_RESULTS);
            TheGame::instance->InitializeAssemblyResultsState();
            GameMode::GetCurrent()->CleanUp();
            TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
        }, TRANSITION_TIME_SECONDS);

        BeginTransitioning();
        m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
        m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
        AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyPlaying() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FEEDFACE);
    SpriteGameRenderer::instance->Render();
    RenderSplitscreenLines();
    RenderDebug();
}

//-----------------------------------------------------------------------------------
void TheGame::RenderSplitscreenLines() const
{
    static const float LINE_WIDTH = 5.0f;
    Renderer::instance->BeginOrtho(SpriteGameRenderer::instance->m_windowVirtualWidth, SpriteGameRenderer::instance->m_windowVirtualHeight, Vector2::ZERO);
    {
        if (m_numberOfPlayers == 2)
        {
            SpriteGameRenderer::instance->DrawLine(Vector2(0.0f, 20.0f), Vector2(0.0f, -20.0f), RGBA::WHITE, LINE_WIDTH);
        }
        else if (m_numberOfPlayers == 3)
        {
            float offsetUnit = SpriteGameRenderer::instance->m_windowVirtualWidth / 6.0f;
            SpriteGameRenderer::instance->DrawLine(Vector2(-offsetUnit, 20.0f), Vector2(-offsetUnit, -20.0f), RGBA::WHITE, LINE_WIDTH);
            SpriteGameRenderer::instance->DrawLine(Vector2(offsetUnit, 20.0f), Vector2(offsetUnit, -20.0f), RGBA::WHITE, LINE_WIDTH);
        }
        else if (m_numberOfPlayers == 4)
        {
            SpriteGameRenderer::instance->DrawLine(Vector2(0.0f, 20.0f), Vector2(0.0f, -20.0f), RGBA::WHITE, LINE_WIDTH);
            SpriteGameRenderer::instance->DrawLine(Vector2(20.0f, 0.0f), Vector2(-20.0f, 0.0f), RGBA::WHITE, LINE_WIDTH);
        }
    }
    Renderer::instance->EndOrtho();
}

//-----------------------------------------------------------------------------------
//ASSEMBLY RESULTS/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyResultsState()
{
    SpriteGameRenderer::instance->SetSplitscreen(4);
    if (!g_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_resultsMusic, 0.6f);
    }
    m_currentGameMode->HideBackground();
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyResultsState);

    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        PlayerShip* ship = TheGame::instance->m_players[i];
        ship->Respawn();
        ship->m_shieldSprite->Disable();
        ship->m_sprite->Disable();
        ship->m_shipTrail->Disable();
        ship->LockMovement();
        ship->ShowStatGraph();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyResultsState(unsigned int)
{
    EnqueueMinigames();
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->m_isDead = false;
        ship->Respawn();
        ship->m_shieldSprite->Enable();
        ship->m_sprite->Enable();
        ship->m_shipTrail->Enable();
        ship->UnlockMovement();
        ship->HideStatGraph();
    }
    m_currentGameMode->HideBackground();
    delete m_currentGameMode;
    m_currentGameMode = m_queuedMinigameModes.front();
    m_queuedMinigameModes.pop();
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->StopSound(m_resultsMusic);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyResults(float deltaSeconds)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->Update(deltaSeconds);
    }

    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        if (IsTransitioningStates())
        {
            return;
        }

        RunAfterSeconds([]()
        {
            SetGameState(MINIGAME_GET_READY);
            TheGame::instance->InitializeMinigameGetReadyState();
            TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
        }, TRANSITION_TIME_SECONDS);

        BeginTransitioning();
        m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
        m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
        AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyResults() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBLIGHTGREEN);
    SpriteGameRenderer::instance->Render();
    RenderSplitscreenLines();
}

//-----------------------------------------------------------------------------------
//MINIGAME GET READY/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigameGetReadyState()
{
    m_currentGameMode->InitializeReadyAnim();
    m_currentGameMode->HideBackground();

    SpriteGameRenderer::instance->DisableAllLayers();
    SpriteGameRenderer::instance->EnableLayer(TEXT_LAYER);
    SpriteGameRenderer::instance->EnableLayer(UI_LAYER);
    SpriteGameRenderer::instance->EnableLayer(FULL_SCREEN_EFFECT_LAYER);

    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigameGetReadyState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigameGetReadyState(unsigned int)
{
    SpriteGameRenderer::instance->EnableAllLayers();
    m_currentGameMode->CleanupReadyAnim();
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigameGetReady(float deltaSeconds)
{
    m_currentGameMode->UpdateReadyAnim(deltaSeconds);
    if (g_secondsInState < TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI || IsTransitioningStates())
    {
        return;
    }
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept") || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
        {
            RunAfterSeconds([]()
            {
                SetGameState(MINIGAME_PLAYING);
                TheGame::instance->InitializeMinigamePlayingState();
                TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
            }, TRANSITION_TIME_SECONDS);

            BeginTransitioning();
            m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
            m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
            m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
            AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
            return;
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMinigameGetReady() const
{
    SpriteGameRenderer::instance->SetClearColor(m_currentGameMode->m_readyBGColor);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//MINIGAME PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigamePlayingState()
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->ShowUI();
    }
    m_currentGameMode->Initialize(m_players);
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigamePlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigamePlayingState(unsigned int)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->HideUI();
    }
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigamePlaying(float deltaSeconds)
{
    CheckForGamePaused();
    if (g_isGamePaused)
    {
        return;
    }

    m_currentGameMode->Update(deltaSeconds);

    if (IsTransitioningStates())
    {
        return;
    }
    if (!m_currentGameMode->m_isPlaying || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
    {
        RunAfterSeconds([]()
        {
            SetGameState(MINIGAME_RESULTS);
            TheGame::instance->InitializeMinigameResultsState();
            GameMode::GetCurrent()->CleanUp();
            TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
        }, TRANSITION_TIME_SECONDS);

        BeginTransitioning();
        m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
        m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
        AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMinigamePlaying() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FEEDFACE);
    SpriteGameRenderer::instance->Render();
    RenderSplitscreenLines();
    RenderDebug();
}

//-----------------------------------------------------------------------------------
//MINIGAME RESULTS/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigameResultsState()
{
    if (!g_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_resultsMusic, 0.6f);
    }
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    m_currentGameMode->HideBackground();
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigameResultsState);
    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        PlayerShip* ship = TheGame::instance->m_players[i];
        ship->Respawn();
        ship->m_shieldSprite->Disable();
        ship->LockMovement();
        float xMultiplier = i % 2 == 0 ? -1.0f : 1.0f;
        float yMultiplier = i >= 2 ? -1.0f : 1.0f;
        ship->SetPosition(Vector2(3.0f * xMultiplier, 3.0f * yMultiplier));
    }

    m_currentGameMode->DetermineWinners();
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigameResultsState(unsigned int)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->UnlockMovement();
        ship->m_isDead = false;
        ship->Respawn();
        ship->m_sprite->m_tintColor = RGBA::WHITE;
        ship->m_shieldSprite->Enable();
    }
    delete m_currentGameMode;
    if (m_queuedMinigameModes.size() > 0)
    {
        m_currentGameMode = m_queuedMinigameModes.front();
        m_queuedMinigameModes.pop();
        AudioSystem::instance->StopSound(m_resultsMusic);
    }
    else
    {
        m_currentGameMode = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigameResults(float deltaSeconds)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->Update(deltaSeconds);
    }

    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        if (IsTransitioningStates())
        {
            return;
        }
        if (m_queuedMinigameModes.size() > 0)
        {
            RunAfterSeconds([]()
            {
                SetGameState(MINIGAME_GET_READY);
                TheGame::instance->InitializeMinigameGetReadyState();
                TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
            }, TRANSITION_TIME_SECONDS);

            BeginTransitioning();
            m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
            m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
            m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
            AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
        }
        else
        {
            RunAfterSeconds([]()
            {
                SetGameState(GAME_RESULTS_SCREEN);
                TheGame::instance->InitializeGameOverState();
                TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
            }, TRANSITION_TIME_SECONDS);

            BeginTransitioning();
            m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
            m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
            m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
            AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMinigameResults() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBLIGHTGREEN);
    SpriteGameRenderer::instance->Render();
}


//-----------------------------------------------------------------------------------
//GAME OVER/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeGameOverState()
{
    m_gameOverText = new Sprite("GameOverText", PLAYER_LAYER);
    m_gameOverText->m_transform.SetScale(Vector2(10.0f, 10.0f));
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupGameOverState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupGameOverState(unsigned int)
{
    delete m_gameOverText;
    AudioSystem::instance->StopSound(m_resultsMusic);
    for (PlayerShip* ship : m_players)
    {
        delete ship;
    }
    m_players.clear();
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateGameOver(float )
{
    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        if (IsTransitioningStates())
        {
            return;
        }
        RunAfterSeconds([]()
        {
            SetGameState(MAIN_MENU);
            TheGame::instance->InitializeMainMenuState();
            TheGame::instance->m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeLeft")->m_texture);
        }, TRANSITION_TIME_SECONDS);

        BeginTransitioning();
        m_transitionFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("PixelWipeRight")->m_texture);
        m_transitionFBOEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        m_transitionFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
        AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderGameOver() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::DISEASED);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
void TheGame::RenderDebug() const
{
    if (InputSystem::instance->IsKeyDown('B'))
    {
        Renderer::instance->BeginOrtho(SpriteGameRenderer::instance->m_virtualWidth, SpriteGameRenderer::instance->m_virtualHeight, SpriteGameRenderer::instance->m_cameraPosition);
        {
            for (Entity* ent : m_currentGameMode->m_entities)
            {
                SpriteGameRenderer::instance->DrawPolygonOutline(ent->m_transform.GetWorldPosition(), ent->m_collisionRadius, 20, 0);
            }

            if (InputSystem::instance->IsKeyDown('R'))
            {
                float radius = 5.0f;
                SpriteGameRenderer::instance->DrawPolygonOutline(m_players[0]->m_transform.GetWorldPosition(), radius, 20, 0);
            }
        }
        Renderer::instance->EndOrtho();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::CheckForGamePaused()
{
    for (PlayerShip* ship : m_players)
    {
        if (!ship->m_isDead && ship->m_pilot->m_inputMap.FindInputValue("Pause")->WasJustPressed())
        {
            g_isGamePaused = !g_isGamePaused;
            if (g_isGamePaused)
            {
                SpriteGameRenderer::instance->AddEffectToLayer(m_pauseFBOEffect, FULL_SCREEN_EFFECT_LAYER);
                GameMode::GetCurrent()->MarkTimerPaused();
                for (PlayerShip* player : m_players)
                {
                    player->ShowStatGraph();
                }
            }
            else
            {
                SpriteGameRenderer::instance->RemoveEffectFromLayer(m_pauseFBOEffect, FULL_SCREEN_EFFECT_LAYER);
                //m_gamePausedLabel->SetHidden();
                for (PlayerShip* player : m_players)
                {
                    player->HideStatGraph();
                }
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RunAfterSeconds(RunAfterSecondsFunction* functionPointer, float secondsToWait)
{
    m_runAfterSecondsCallbackFunctions.emplace_back(functionPointer, secondsToWait);
}

//-----------------------------------------------------------------------------------
void TheGame::DispatchRunAfterSeconds()
{
    for (auto iter = m_runAfterSecondsCallbackFunctions.begin(); iter != m_runAfterSecondsCallbackFunctions.end();)
    {
        RunAfterSecondsCallback& callbackData = *iter;
        if (GetCurrentTimeSeconds() - callbackData.m_dispatchedTimestampSeconds > callbackData.m_secondsToWait)
        {
            callbackData.m_functionPointer();
            iter = m_runAfterSecondsCallbackFunctions.erase(iter);
        }
        if (iter == m_runAfterSecondsCallbackFunctions.end())
        {
            break;
        }
        ++iter;
    }
}

//-----------------------------------------------------------------------------------
void TheGame::InitializeSpriteLayers()
{
    SpriteGameRenderer::instance->CreateOrGetLayer(TEXT_LAYER)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(TEXT_LAYER)->m_isCullingEnabled = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(FBO_FREE_TEXT_LAYER)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(FBO_FREE_TEXT_LAYER)->m_isCullingEnabled = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_UI_LAYER)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_UI_LAYER)->m_isCullingEnabled = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(UI_LAYER)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(UI_LAYER)->m_isCullingEnabled = false;

    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_PARTICLES_BLOOM_LAYER)->m_isBloomEnabled = true;
    SpriteGameRenderer::instance->CreateOrGetLayer(BULLET_LAYER_BLOOM)->m_isBloomEnabled = true;
    SpriteGameRenderer::instance->CreateOrGetLayer(STAT_GRAPH_LAYER)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(STAT_GRAPH_LAYER_BACKGROUND)->m_isWorldSpaceLayer = false;
    SpriteGameRenderer::instance->CreateOrGetLayer(STAT_GRAPH_LAYER_TEXT)->m_isWorldSpaceLayer = false;
}

//-----------------------------------------------------------------------------------
void TheGame::InitializeKeyMappingsForPlayer(PlayerPilot* playerPilot)
{
    if (playerPilot->m_controllerIndex == -1)
    {
        KeyboardInputDevice* keyboard = InputSystem::instance->m_keyboardDevice;
        MouseInputDevice* mouse = InputSystem::instance->m_mouseDevice;
        //KEYBOARD & MOUSE INPUT
        playerPilot->m_inputMap.MapInputAxis("Up", keyboard->FindValue('W'), keyboard->FindValue('S'));
        playerPilot->m_inputMap.MapInputAxis("Right", keyboard->FindValue('D'), keyboard->FindValue('A'));
        playerPilot->m_inputMap.MapInputAxis("ShootUp", &mouse->m_deltaPosition.m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootRight", &mouse->m_deltaPosition.m_xAxis);
        playerPilot->m_inputMap.MapInputValue("Suicide", keyboard->FindValue('K'));
        playerPilot->m_inputMap.MapInputValue("Shoot", keyboard->FindValue(' '));
        playerPilot->m_inputMap.MapInputValue("Shoot", mouse->FindButtonValue(InputSystem::MouseButton::LEFT_MOUSE_BUTTON));
        playerPilot->m_inputMap.MapInputValue("Activate", mouse->FindButtonValue(InputSystem::MouseButton::RIGHT_MOUSE_BUTTON));
        playerPilot->m_inputMap.MapInputValue("Accept", keyboard->FindValue(InputSystem::ExtraKeys::ENTER));
        playerPilot->m_inputMap.MapInputValue("Accept", keyboard->FindValue(' '));
        playerPilot->m_inputMap.MapInputValue("Back", keyboard->FindValue(InputSystem::ExtraKeys::BACKSPACE));
        playerPilot->m_inputMap.MapInputValue("Back", keyboard->FindValue('B'));
        playerPilot->m_inputMap.MapInputValue("Respawn", keyboard->FindValue(' '));
        playerPilot->m_inputMap.MapInputValue("Respawn", keyboard->FindValue('R'));
        playerPilot->m_inputMap.MapInputValue("Respawn", keyboard->FindValue('P'));
        playerPilot->m_inputMap.MapInputValue("EjectActive", keyboard->FindValue('Z'));
        playerPilot->m_inputMap.MapInputValue("EjectWeapon", keyboard->FindValue('X'));
        playerPilot->m_inputMap.MapInputValue("EjectPassive", keyboard->FindValue('C'));
        playerPilot->m_inputMap.MapInputValue("EjectChassis", keyboard->FindValue('V'));
        playerPilot->m_inputMap.MapInputValue("Pause", keyboard->FindValue('P'));
        playerPilot->m_inputMap.MapInputValue("CycleColorsLeft", keyboard->FindValue('A'));
        playerPilot->m_inputMap.MapInputValue("CycleColorsRight", keyboard->FindValue('D'));
    }
    else
    {
        XInputDevice* controller = InputSystem::instance->m_xInputDevices[playerPilot->m_controllerIndex];
        //CONTROLLER INPUT
        playerPilot->m_inputMap.MapInputAxis("Up")->AddMapping(&controller->GetLeftStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("Right")->AddMapping(&controller->GetLeftStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootRight")->AddMapping(&controller->GetRightStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootUp")->AddMapping(&controller->GetRightStick()->m_yAxis);
        playerPilot->m_inputMap.FindInputAxis("ShootRight")->m_deadzoneValue = 0.3f;
        playerPilot->m_inputMap.FindInputAxis("ShootUp")->m_deadzoneValue = 0.3f;
        playerPilot->m_inputMap.MapInputValue("Shoot", ChordResolutionMode::RESOLVE_MAXS_ABSOLUTE)->m_deadzoneValue = XInputController::INNER_DEADZONE;
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightTrigger());
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightStickMagnitude());
        playerPilot->m_inputMap.MapInputValue("Activate", controller->GetLeftTrigger());
        playerPilot->m_inputMap.MapInputValue("EjectActive", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("EjectWeapon", controller->FindButton(XboxButton::B));
        playerPilot->m_inputMap.MapInputValue("EjectPassive", controller->FindButton(XboxButton::X));
        playerPilot->m_inputMap.MapInputValue("EjectChassis", controller->FindButton(XboxButton::Y));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("Back", controller->FindButton(XboxButton::B));
        playerPilot->m_inputMap.MapInputValue("Back", controller->FindButton(XboxButton::BACK));
        playerPilot->m_inputMap.MapInputValue("Respawn", controller->FindButton(XboxButton::BACK));
        playerPilot->m_inputMap.MapInputValue("Respawn", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("Pause", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("CycleColorsLeft", controller->FindButton(XboxButton::X));
        playerPilot->m_inputMap.MapInputValue("CycleColorsRight", controller->FindButton(XboxButton::Y));
        playerPilot->m_inputMap.MapInputValue("CycleColorsLeft", controller->FindButton(XboxButton::LB));
        playerPilot->m_inputMap.MapInputValue("CycleColorsRight", controller->FindButton(XboxButton::RB));
        playerPilot->m_inputMap.MapInputValue("CycleColorsLeft", controller->GetLeftTrigger());
        playerPilot->m_inputMap.MapInputValue("CycleColorsRight", controller->GetRightTrigger());
    }
}

//-----------------------------------------------------------------------------------
void TheGame::PreloadAudio()
{
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/cratePop.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Missle_02.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_07.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_08.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/warp.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/swapDimensions.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/trashExplosion.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_04.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_05.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/UI/UI_Select_01.wav");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_1.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_2.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_3.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_4.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_5.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/time_up.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Function - 02 Acylite.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Sweet Tooth - 04 Strawberry.ogg");
    AudioSystem::instance->CreateOrGetSound("Data/Music/Overcast.ogg");
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSprites()
{
    //Debug
    ResourceDatabase::instance->RegisterSprite("Twah", "Data\\Images\\Twah.png");
    ResourceDatabase::instance->RegisterSprite("Quad", "Data\\Images\\whitePixel.png");
    ResourceDatabase::instance->RegisterSprite("Cloudy", "Data\\Images\\Particles\\Cloudy_Thicc.png");
    ResourceDatabase::instance->RegisterSprite("ReadyText", "Data\\Images\\ready.png");

    //UI
    ResourceDatabase::instance->RegisterSprite("Arrow", "Data\\Images\\UI\\Arrow.png");
    ResourceDatabase::instance->RegisterSprite("EquipmentUI", "Data\\Images\\UI\\equipmentUI.png");
    ResourceDatabase::instance->EditSpriteResource("EquipmentUI")->m_pivotPoint = Vector2(1.0f, 0.0f);
    ResourceDatabase::instance->RegisterSprite("HealthUI", "Data\\Images\\UI\\healthBarUI.png");
    ResourceDatabase::instance->EditSpriteResource("HealthUI")->m_pivotPoint = Vector2::ZERO;
    ResourceDatabase::instance->RegisterSprite("EmptyEquipSlot", "Data\\Images\\UI\\emptyShield.png");

    //Transitions
    ResourceDatabase::instance->RegisterSprite("WipeUpAndDown", "Data\\Images\\Transitions\\wipeUpAndDown.png");
    ResourceDatabase::instance->RegisterSprite("WipeLeftAndRight", "Data\\Images\\Transitions\\wipeLeftAndRight.png");
    ResourceDatabase::instance->RegisterSprite("WipeLeft", "Data\\Images\\Transitions\\wipeLeft.png");
    ResourceDatabase::instance->RegisterSprite("PixelWipeLeft", "Data\\Images\\Transitions\\pixelWipeLeft.png");
    ResourceDatabase::instance->RegisterSprite("WipeRight", "Data\\Images\\Transitions\\wipeRight.png");
    ResourceDatabase::instance->RegisterSprite("PixelWipeRight", "Data\\Images\\Transitions\\pixelWipeRight.png");
    ResourceDatabase::instance->RegisterSprite("AngularWipe", "Data\\Images\\Transitions\\angularWipe.png");
    ResourceDatabase::instance->RegisterSprite("PixelAngularWipe", "Data\\Images\\Transitions\\pixelAngularWipe.png");
    ResourceDatabase::instance->RegisterSprite("BlurAngularWipe", "Data\\Images\\Transitions\\blurWipe.png");
    ResourceDatabase::instance->RegisterSprite("StarWipe", "Data\\Images\\Transitions\\starWipe.png");
    ResourceDatabase::instance->RegisterSprite("PixelStarWipe", "Data\\Images\\Transitions\\pixelStarWipe.png");
    ResourceDatabase::instance->RegisterSprite("SpiralWipe", "Data\\Images\\Transitions\\spiralWipe.png");
    ResourceDatabase::instance->RegisterSprite("SlashWipe", "Data\\Images\\Transitions\\slashWipe.png");
    ResourceDatabase::instance->RegisterSprite("PixelSlashWipe", "Data\\Images\\Transitions\\pixelSlashWipe.png");
    ResourceDatabase::instance->RegisterSprite("ReadyScreen", "Data\\Images\\Transitions\\readyScreen.png");

    //Backgrounds
    ResourceDatabase::instance->RegisterSprite("DefaultBackground", "Data\\Images\\Backgrounds\\bg1.jpg");
    ResourceDatabase::instance->RegisterSprite("Assembly", "Data\\Images\\Backgrounds\\bg1.jpg");
    ResourceDatabase::instance->RegisterSprite("BattleBackground", "Data\\Images\\Backgrounds\\bg2.jpg");
    ResourceDatabase::instance->RegisterSprite("Starfield", "Data\\Images\\Starfield_Foreground.png");
    ResourceDatabase::instance->EditSpriteResource("Starfield")->m_uvBounds = AABB2(Vector2(-15.0f), Vector2(15.0f));

    //Entities
    ResourceDatabase::instance->RegisterSprite("MuzzleFlash", "Data\\Images\\Lasers\\muzzleFlash.png");
    ResourceDatabase::instance->RegisterSprite("Pico", "Data\\Images\\Pico.png");
    ResourceDatabase::instance->RegisterSprite("Shield", "Data\\Images\\Shield.png");
    ResourceDatabase::instance->RegisterSprite("GameOverText", "Data\\Images\\GameOver.png");
    ResourceDatabase::instance->RegisterSprite("Invalid", "Data\\Images\\invalidSpriteResource.png");

    //Props
    ResourceDatabase::instance->RegisterSprite("Asteroid", "Data\\Images\\Props\\asteroid01.png");
    ResourceDatabase::instance->RegisterSprite("Nebula", "Data\\Images\\Props\\Nebula.png");
    ResourceDatabase::instance->RegisterSprite("Nebula2", "Data\\Images\\Props\\Nebula2.png");
    ResourceDatabase::instance->RegisterSprite("Wormhole", "Data\\Images\\Props\\cheapVortex3.png");
    ResourceDatabase::instance->RegisterSprite("ItemBox", "Data\\Images\\Props\\itemCrate.png");

    //Enemies
    ResourceDatabase::instance->RegisterSprite("Grunt", "Data\\Images\\Enemies\\grunt.png");

    //Projectiles
    ResourceDatabase::instance->RegisterSprite("Laser", "Data\\Images\\Lasers\\laserColorless.png");
    ResourceDatabase::instance->RegisterSprite("Missile1", "Data\\Images\\Lasers\\basicMissile.png");
    ResourceDatabase::instance->RegisterSprite("Missile2", "Data\\Images\\Lasers\\basicMissile2.png");
    ResourceDatabase::instance->RegisterSprite("PlasmaBall", "Data\\Images\\Lasers\\plasmaBall.png");
    SpriteResource* plasmaBall = ResourceDatabase::instance->EditSpriteResource("PlasmaBall");
    delete plasmaBall->m_defaultMaterial;
    plasmaBall->m_defaultMaterial = new Material(SpriteGameRenderer::instance->m_defaultShader, SpriteGameRenderer::instance->m_additiveBlendRenderState);

    //Color Palettes:
    ResourceDatabase::instance->RegisterSprite("ShipColorPalettes", "Data\\Images\\Palettes\\shipPalettes.png");
    ResourceDatabase::instance->RegisterSprite("ColorPalettes", "Data\\Images\\Palettes\\gameboyPalettes.png");

    //Chassis
    ResourceDatabase::instance->RegisterSprite("DefaultChassis", "Data\\Images\\Chassis\\defaultChassis.png");
    ResourceDatabase::instance->RegisterSprite("SpeedChassis", "Data\\Images\\Chassis\\speedChassis.png");
    ResourceDatabase::instance->RegisterSprite("PowerChassis", "Data\\Images\\Chassis\\speedChassis.png");
    ResourceDatabase::instance->RegisterSprite("TankChassis", "Data\\Images\\Chassis\\speedChassis.png");
    ResourceDatabase::instance->RegisterSprite("GlassCannonChassis", "Data\\Images\\Chassis\\speedChassis.png");
    ResourceDatabase::instance->RegisterSprite("BlackHoleChassis", "Data\\Images\\Chassis\\blackHoleChassis.png");

    //Chassis Pickups
    ResourceDatabase::instance->RegisterSprite("DefaultChassisPickup", "Data\\Images\\Chassis\\normalPickup.png");
    ResourceDatabase::instance->RegisterSprite("SpeedChassisPickup", "Data\\Images\\Chassis\\speedPickup.png");
    ResourceDatabase::instance->RegisterSprite("PowerChassisPickup", "Data\\Images\\Chassis\\speedPickup.png");
    ResourceDatabase::instance->RegisterSprite("TankChassisPickup", "Data\\Images\\Chassis\\speedPickup.png");
    ResourceDatabase::instance->RegisterSprite("GlassCannonChassisPickup", "Data\\Images\\Chassis\\speedPickup.png");
    ResourceDatabase::instance->RegisterSprite("BlackHoleChassisPickup", "Data\\Images\\Chassis\\blackHolePickup.png");

    //Passive Pickups
    ResourceDatabase::instance->RegisterSprite("CloakPassive", "Data\\Images\\Passives\\cloakPassive.png");
    ResourceDatabase::instance->RegisterSprite("StealthTrailPassive", "Data\\Images\\Passives\\stealthTrailPassive.png");
    ResourceDatabase::instance->RegisterSprite("SprayAndPrayPassive", "Data\\Images\\Passives\\sprayAndPrayPassive.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailPassiveLol", "Data\\Images\\Passives\\specialTrailPassiveLol.png");

    //Active Pickups
    ResourceDatabase::instance->RegisterSprite("WarpActive", "Data\\Images\\Actives\\warpActive.png");
    ResourceDatabase::instance->RegisterSprite("TeleportActive", "Data\\Images\\Actives\\teleportActive.png");
    ResourceDatabase::instance->RegisterSprite("QuickshotActive", "Data\\Images\\Actives\\quickshotActive.png");
    ResourceDatabase::instance->RegisterSprite("ShieldActive", "Data\\Images\\Actives\\shieldActive.png");
    ResourceDatabase::instance->RegisterSprite("BoostActive", "Data\\Images\\Actives\\boostActive.png");

    //Weapon Pickups
    ResourceDatabase::instance->RegisterSprite("MissileLauncher", "Data\\Images\\Weapons\\missileLauncher.png");
    ResourceDatabase::instance->RegisterSprite("DefaultWeapon", "Data\\Images\\Weapons\\defaultWeapon.png");
    ResourceDatabase::instance->RegisterSprite("SpreadShot", "Data\\Images\\Weapons\\spreadShot.png");
    ResourceDatabase::instance->RegisterSprite("WaveGun", "Data\\Images\\Weapons\\waveGun.png");

    //Pickups
    ResourceDatabase::instance->RegisterSprite("Top Speed", "Data\\Images\\Pickups\\speed.png");
    ResourceDatabase::instance->RegisterSprite("Acceleration", "Data\\Images\\Pickups\\acceleration.png");
    ResourceDatabase::instance->RegisterSprite("Handling", "Data\\Images\\Pickups\\handling.png");
    ResourceDatabase::instance->RegisterSprite("Braking", "Data\\Images\\Pickups\\braking.png");
    ResourceDatabase::instance->RegisterSprite("Damage", "Data\\Images\\Pickups\\damage.png");
    ResourceDatabase::instance->RegisterSprite("Shield Disruption", "Data\\Images\\Pickups\\shieldDisruption.png");
    ResourceDatabase::instance->RegisterSprite("Shot Homing", "Data\\Images\\Pickups\\shotHoming.png");
    ResourceDatabase::instance->RegisterSprite("Rate Of Fire", "Data\\Images\\Pickups\\fireRate.png");
    ResourceDatabase::instance->RegisterSprite("Hp", "Data\\Images\\Pickups\\hp.png");
    ResourceDatabase::instance->RegisterSprite("Shield Capacity", "Data\\Images\\Pickups\\shieldCapacity.png");
    ResourceDatabase::instance->RegisterSprite("Shield Regen", "Data\\Images\\Pickups\\shieldRegen.png");
    ResourceDatabase::instance->RegisterSprite("Shot Deflection", "Data\\Images\\Pickups\\shotDeflection.png");

    //Trails
    ResourceDatabase::instance->RegisterSprite("BeamTrail", "Data\\Images\\Particles\\beamTrail.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailVStripe", "Data\\Images\\Particles\\vStripeTrail.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailRacingStripe", "Data\\Images\\Particles\\racingStripeTrail.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailVarying", "Data\\Images\\Particles\\varyingTrail.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailLol", "Data\\Images\\Particles\\lolTrail.png");
    ResourceDatabase::instance->RegisterSprite("SpecialTrailBubble", "Data\\Images\\Particles\\bitTrail.png");

    //Particles
    ResourceDatabase::instance->RegisterSprite("Placeholder", "Data\\Images\\Particles\\placeholder.png");
    ResourceDatabase::instance->RegisterSprite("ParticleBeige", "Data\\Images\\Particles\\particle_beige.png");
    ResourceDatabase::instance->RegisterSprite("ParticleBlue", "Data\\Images\\Particles\\particle_blue.png");
    ResourceDatabase::instance->RegisterSprite("ParticleBrown", "Data\\Images\\Particles\\particle_brown.png");
    ResourceDatabase::instance->RegisterSprite("ParticleDarkBrown", "Data\\Images\\Particles\\particle_darkBrown.png");
    ResourceDatabase::instance->RegisterSprite("ParticleDarkGrey", "Data\\Images\\Particles\\particle_darkGrey.png");
    ResourceDatabase::instance->RegisterSprite("ParticleGreen", "Data\\Images\\Particles\\particle_green.png");
    ResourceDatabase::instance->RegisterSprite("ParticleGrey", "Data\\Images\\Particles\\particle_grey.png");
    ResourceDatabase::instance->RegisterSprite("ParticlePink", "Data\\Images\\Particles\\particle_pink.png");
    ResourceDatabase::instance->RegisterSprite("BlackSmoke", "Data\\Images\\Particles\\blackSmoke01.png");
    ResourceDatabase::instance->RegisterSprite("Explosion", "Data\\Images\\Particles\\explosion08.png");
    ResourceDatabase::instance->RegisterSprite("BlueWarp", "Data\\Images\\Particles\\particleBlue_2.png");
    ResourceDatabase::instance->RegisterSprite("WhiteBeam", "Data\\Images\\Particles\\particleWhite_5.png");
    ResourceDatabase::instance->RegisterSprite("White4Star", "Data\\Images\\Particles\\particleWhite_7.png");
    ResourceDatabase::instance->RegisterSprite("White8Star", "Data\\Images\\Particles\\particleWhite_6.png");
    ResourceDatabase::instance->RegisterSprite("Blue4Star", "Data\\Images\\Particles\\particleBlue_7.png");
    ResourceDatabase::instance->RegisterSprite("Yellow4Star", "Data\\Images\\Particles\\particleYellow_7.png");
    ResourceDatabase::instance->RegisterSprite("Yellow5Star", "Data\\Images\\Particles\\particleYellow_3.png");
    ResourceDatabase::instance->RegisterSprite("YellowCircle", "Data\\Images\\Particles\\particleYellow_8.png");
    ResourceDatabase::instance->RegisterSprite("YellowBeam", "Data\\Images\\Particles\\particleYellow_9.png");
    ResourceDatabase::instance->RegisterSprite("BlueShieldHex", "Data\\Images\\Particles\\overshield.png");
    ResourceDatabase::instance->EditSpriteResource("YellowBeam")->m_pivotPoint.y = 0.0f;

}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSpriteAnimations()
{
    SpriteAnimationResource* missileAnimation = ResourceDatabase::instance->RegisterSpriteAnimation("Missile", SpriteAnimationLoopMode::LOOP);
    missileAnimation->AddFrame("Missile1", 0.05f);
    missileAnimation->AddFrame("Missile2", 0.05f);
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterParticleEffects()
{
    const float POWER_UP_DURATION = 5.0f;
    const float BOOST_DURATION = 1.0f;
    const float DEATH_ANIMATION_LENGTH = 1.5f;
    const float WARP_ANIMATION_LENGTH = 1.5f;
    const float POWER_UP_PICKUP_ANIMATION_LENGTH = 0.15f;
    const float MUZZLE_FLASH_ANIMATION_LENGTH = 0.01f;
    const float CRATE_DESTRUCTION_ANIMATION_LENGTH = 0.6f;
    const float COLLISION_ANIMATION_LENGTH = 0.3f;

    //EMITTERS/////////////////////////////////////////////////////////////////////
    ParticleEmitterDefinition* white8Stars = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("White8Star"));
    white8Stars->m_properties.Set<std::string>(PROPERTY_NAME, "Sparkly 8Star Power");
    white8Stars->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    white8Stars->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(10, 15));
    white8Stars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    white8Stars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    white8Stars->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.2f);
    white8Stars->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, POWER_UP_DURATION);
    white8Stars->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 40.0f);
    white8Stars->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    white8Stars->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-0.3f));

    ParticleEmitterDefinition* boostEffect = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("WhiteBeam"));
    boostEffect->m_properties.Set<std::string>(PROPERTY_NAME, "Boost Effect");
    boostEffect->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    boostEffect->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(10, 15));
    boostEffect->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    boostEffect->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    boostEffect->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.2f);
    boostEffect->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, BOOST_DURATION);
    boostEffect->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 40.0f);
    boostEffect->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    boostEffect->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-0.3f));

    ParticleEmitterDefinition* blueStars = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("Blue4Star"));
    blueStars->m_properties.Set<std::string>(PROPERTY_NAME, "Blue Stars");
    blueStars->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    blueStars->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(10, 15));
    blueStars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    blueStars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    blueStars->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.2f);
    blueStars->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, WARP_ANIMATION_LENGTH);
    blueStars->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 40.0f);
    blueStars->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    blueStars->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(1.3f));

    ParticleEmitterDefinition* blueWarpOrb = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BlueWarp"));
    blueWarpOrb->m_properties.Set<std::string>(PROPERTY_NAME, "Blue Warp");
    blueWarpOrb->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    blueWarpOrb->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    blueWarpOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(1.8f), Vector2(2.0f)));
    blueWarpOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    blueWarpOrb->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, WARP_ANIMATION_LENGTH);
    blueWarpOrb->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ANGULAR_VELOCITY_DEGREES, Range<float>(180.0f, 270.0f));
    blueWarpOrb->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    blueWarpOrb->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-1.5f, -1.5f));

    ParticleEmitterDefinition* yellowStars = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("Yellow4Star"));
    yellowStars->m_properties.Set<std::string>(PROPERTY_NAME, "Yellow Stars");
    yellowStars->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    yellowStars->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(10, 15));
    yellowStars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    yellowStars->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    yellowStars->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.2f);
    yellowStars->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, DEATH_ANIMATION_LENGTH);
    yellowStars->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 40.0f);
    yellowStars->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    yellowStars->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(1.3f));

    ParticleEmitterDefinition* yellowExplosionOrb = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("YellowCircle"));
    yellowExplosionOrb->m_properties.Set<bool>(PROPERTY_LOCK_PARTICLES_TO_EMITTER, true);
    yellowExplosionOrb->m_properties.Set<std::string>(PROPERTY_NAME, "Yellow Explosion Orb");
    yellowExplosionOrb->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    yellowExplosionOrb->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    yellowExplosionOrb->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, DEATH_ANIMATION_LENGTH);
    yellowExplosionOrb->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(1.3f));

    ParticleEmitterDefinition* blueShieldHex = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BlueShieldHex"));
    blueShieldHex->m_properties.Set<std::string>(PROPERTY_NAME, "Blue Shield Hex");
    blueShieldHex->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    blueShieldHex->m_properties.Set<bool>(PROPERTY_LOCK_PARTICLES_TO_EMITTER, true);
    blueShieldHex->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    blueShieldHex->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.5f), Vector2(0.7f)));
    blueShieldHex->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    blueShieldHex->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 1.0f);
    blueShieldHex->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, POWER_UP_DURATION);
    blueShieldHex->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 4.0f);
    blueShieldHex->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(4.0f));

    ParticleEmitterDefinition* yellowBeams = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("YellowBeam"));
    yellowBeams->m_properties.Set<bool>(PROPERTY_LOCK_PARTICLES_TO_EMITTER, true);
    yellowBeams->m_properties.Set<std::string>(PROPERTY_NAME, "Yellow Beams");
    yellowBeams->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    yellowBeams->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(5, 10));
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    yellowBeams->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, DEATH_ANIMATION_LENGTH);
    yellowBeams->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.0f, 2.0f));
    yellowBeams->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    yellowBeams->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ANGULAR_VELOCITY_DEGREES, Range<float>(-30.0f, 30.0f));

    ParticleEmitterDefinition* powerupPickup = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("Placeholder"));
    powerupPickup->m_properties.Set<std::string>(PROPERTY_NAME, "Power Pickup");
    powerupPickup->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    powerupPickup->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(5, 15));
    powerupPickup->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    powerupPickup->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::UNIT_Y);
    powerupPickup->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.5f);
    powerupPickup->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    powerupPickup->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, POWER_UP_PICKUP_ANIMATION_LENGTH);
    powerupPickup->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    powerupPickup->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.3f));

    ParticleEmitterDefinition* muzzleFlash = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("MuzzleFlash"));
    muzzleFlash->m_properties.Set<std::string>(PROPERTY_NAME, "Muzzle Flash");
    muzzleFlash->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    muzzleFlash->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(2, 4));
    muzzleFlash->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.6f), Vector2(0.8f)));
    muzzleFlash->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    muzzleFlash->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, MUZZLE_FLASH_ANIMATION_LENGTH);
    muzzleFlash->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    muzzleFlash->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, MUZZLE_FLASH_ANIMATION_LENGTH);
    muzzleFlash->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    muzzleFlash->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-0.3f));

    ParticleEmitterDefinition* crateDestroyed = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("ParticleBeige"));
    crateDestroyed->m_properties.Set<std::string>(PROPERTY_NAME, "Crate Destroyed");
    //crateDestroyed->m_properties.Set<float>("Gravity Scale", 1.0f);
    crateDestroyed->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    crateDestroyed->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    crateDestroyed->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(5, 15));
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_EXPLOSIVE_VELOCITY_MAGNITUDE, 5.0f);
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.5f);
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, CRATE_DESTRUCTION_ANIMATION_LENGTH);
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, Range<float>(0.4f, 0.6f));
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    crateDestroyed->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    crateDestroyed->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(1.3f));

    ParticleEmitterDefinition* shipTrail = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BeamTrail"));
    shipTrail->m_properties.Set<std::string>(PROPERTY_NAME, "Ship Trail");
    shipTrail->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    shipTrail->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    shipTrail->m_properties.Set<float>(PROPERTY_WIDTH, 0.25f);
    shipTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    shipTrail->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 1.5f);
    shipTrail->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 20.0f);
    shipTrail->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    shipTrail->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.3f));

    ParticleEmitterDefinition* missileTrail = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BeamTrail"));
    missileTrail->m_properties.Set<std::string>(PROPERTY_NAME, "Missile Trail");
    missileTrail->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    missileTrail->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    missileTrail->m_properties.Set<float>(PROPERTY_WIDTH, 0.125f);
    missileTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    missileTrail->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 1.5f);
    missileTrail->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 10.0f);
    missileTrail->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    missileTrail->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.3f));

    ParticleEmitterDefinition* beamTrail = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BeamTrail"));
    beamTrail->m_properties.Set<std::string>(PROPERTY_NAME, "Beam Trail");
    beamTrail->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    beamTrail->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    beamTrail->m_properties.Set<float>(PROPERTY_WIDTH, 0.125f);
    beamTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    beamTrail->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 0.1f);
    beamTrail->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 40.0f);
    beamTrail->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    beamTrail->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.3f));

    ParticleEmitterDefinition* collisionParticle = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("Cloudy"));
    collisionParticle->m_properties.Set<std::string>(PROPERTY_NAME, "Collision");
    collisionParticle->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    collisionParticle->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    collisionParticle->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(3, 10));
    collisionParticle->m_properties.Set<Range<float>>(PROPERTY_EXPLOSIVE_VELOCITY_MAGNITUDE, 2.0f);
    collisionParticle->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, COLLISION_ANIMATION_LENGTH);
    collisionParticle->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, COLLISION_ANIMATION_LENGTH);
    collisionParticle->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    collisionParticle->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    collisionParticle->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-0.1f));

    ParticleEmitterDefinition* titleScreenParticle = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("White4Star"));
    titleScreenParticle->m_properties.Set<std::string>(PROPERTY_NAME, "TitleScreen");
    titleScreenParticle->m_properties.Set<float>(PROPERTY_GRAVITY_SCALE, -1.0f);
    titleScreenParticle->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    titleScreenParticle->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 30.0f);
    titleScreenParticle->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 30);
    titleScreenParticle->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 10.0f);
    titleScreenParticle->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    titleScreenParticle->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    titleScreenParticle->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ANGULAR_VELOCITY_DEGREES, Range<float>(-360.0f, 360.0f));
    titleScreenParticle->m_properties.Set<Range<float>>(PROPERTY_SPAWN_RADIUS, 10.0f);
    titleScreenParticle->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Range<Vector2>(Vector2(0.0f, 1.0f), Vector2(0.0f, 3.0f)));
    titleScreenParticle->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.4f), Vector2(0.8f)));
    //titleScreenParticle->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(-0.1f));

    ParticleEmitterDefinition* smokeTrail = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BlackSmoke"));
    smokeTrail->m_properties.Set<std::string>(PROPERTY_NAME, "Smoke Trail");
    smokeTrail->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    smokeTrail->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    smokeTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Vector2(0.1f));
    smokeTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    smokeTrail->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 1.0f);
    smokeTrail->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 2.0f);
    smokeTrail->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    smokeTrail->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.5f));
    smokeTrail->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));
    smokeTrail->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ANGULAR_VELOCITY_DEGREES, Range<float>(-90.0f, 90.0f));

    ParticleEmitterDefinition* smoking = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BlackSmoke"));
    smoking->m_properties.Set<std::string>(PROPERTY_NAME, "Crate Destroyed");
    smoking->m_properties.Set<float>("Gravity Scale", -0.1f);
    smoking->m_properties.Set<RGBA>(PROPERTY_INITIAL_COLOR, RGBA(0xFFFFFF44));
    smoking->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    smoking->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(2, 3));
    smoking->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    smoking->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.25f));
    smoking->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 2.0f);
    smoking->m_properties.Set<Range<float>>(PROPERTY_EXPLOSIVE_VELOCITY_MAGNITUDE, 0.25f);
    smoking->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 3.0f);
    smoking->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    smoking->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ANGULAR_VELOCITY_DEGREES, Range<float>(-90.0f, 90.0f));
    smoking->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));

    //SYSTEMS/////////////////////////////////////////////////////////////////////
    ParticleSystemDefinition* deathParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Death", ONE_SHOT);
    deathParticleSystem->AddEmitter(yellowStars);
    deathParticleSystem->AddEmitter(yellowExplosionOrb);
    deathParticleSystem->AddEmitter(yellowBeams);

    ParticleSystemDefinition* smokingParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Smoking", ONE_SHOT);
    smokingParticleSystem->AddEmitter(smoking);

    ParticleSystemDefinition* buffParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Buff", ONE_SHOT);
    buffParticleSystem->AddEmitter(white8Stars);

    ParticleSystemDefinition* boostParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Boost", ONE_SHOT);
    boostParticleSystem->AddEmitter(boostEffect);

    ParticleSystemDefinition* forcefieldParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Forcefield", ONE_SHOT);
    forcefieldParticleSystem->AddEmitter(blueShieldHex);

    ParticleSystemDefinition* warpingParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Warping", ONE_SHOT);
    warpingParticleSystem->AddEmitter(blueStars);

    ParticleSystemDefinition* warpedParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Warped", ONE_SHOT);
    warpedParticleSystem->AddEmitter(blueWarpOrb);

    ParticleSystemDefinition* powerupPickupParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("PowerupPickup", ONE_SHOT);
    powerupPickupParticleSystem->AddEmitter(powerupPickup);

    ParticleSystemDefinition* muzzleFlashParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("MuzzleFlash", ONE_SHOT);
    muzzleFlashParticleSystem->AddEmitter(muzzleFlash);

    ParticleSystemDefinition* crateDestroyedParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("CrateDestroyed", ONE_SHOT);
    crateDestroyedParticleSystem->AddEmitter(crateDestroyed);

    ParticleSystemDefinition* collisionParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Collision", ONE_SHOT);
    collisionParticleSystem->AddEmitter(collisionParticle);

    ParticleSystemDefinition* shipTrailParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("ShipTrail", LOOPING);
    shipTrailParticleSystem->AddEmitter(shipTrail);

    ParticleSystemDefinition* smokeTrailParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("SmokeTrail", LOOPING);
    smokeTrailParticleSystem->AddEmitter(smokeTrail);

    ParticleSystemDefinition* missileTrailParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("MissileTrail", LOOPING);
    missileTrailParticleSystem->AddEmitter(missileTrail);

    ParticleSystemDefinition* beamTrailParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("BeamTrail", LOOPING);
    beamTrailParticleSystem->AddEmitter(beamTrail);

    ParticleSystemDefinition* titleScreenParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Title", LOOPING);
    titleScreenParticleSystem->AddEmitter(titleScreenParticle);
}
