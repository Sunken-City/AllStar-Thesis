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
#include "Game/Entities/Projectile.hpp"
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

TheGame* TheGame::instance = nullptr;

const float TheGame::TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI = 0.5f;

//-----------------------------------------------------------------------------------
TheGame::TheGame()
    : m_currentGameMode(nullptr)
    , SFX_UI_ADVANCE(AudioSystem::instance->CreateOrGetSound("Data/SFX/UI/UI_Select_01.wav"))
{
    ResourceDatabase::instance = new ResourceDatabase();
    RegisterSprites();
    RegisterSpriteAnimations();
    RegisterParticleEffects();
    EventSystem::RegisterObjectForEvent("StartGame", this, &TheGame::PressStart);
    srand(GetTimeBasedSeed());

    m_pauseFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\pixelation.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );
    m_pauseFBOEffect->SetFloatUniform("gPixelationFactor", 16.0f);
    m_rainbowFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\rainbow.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );

    SetGameState(GameState::MAIN_MENU);
    InitializeMainMenuState();
}


//-----------------------------------------------------------------------------------
TheGame::~TheGame()
{
    SetGameState(GameState::SHUTDOWN);

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
    delete ResourceDatabase::instance;
    ResourceDatabase::instance = nullptr;
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
    m_titleText = new TextRenderable2D("GOOD GAME 2017", Transform2D(Vector2(0.0f, 0.0f)), TEXT_LAYER);
    SpriteGameRenderer::instance->AddEffectToLayer(m_rainbowFBOEffect, BACKGROUND_LAYER);
    m_titleParticles = new ParticleSystem("Title", BACKGROUND_LAYER, Vector2(0.0f, -15.0f));
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMainMenuState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMainMenuState(unsigned int)
{
    delete m_titleText;
    ParticleSystem::DestroyImmediately(m_titleParticles);
    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_rainbowFBOEffect, BACKGROUND_LAYER);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMainMenu(float )
{
    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        NamedProperties properties;
        PressStart(properties);
    }
    m_titleText->m_transform.rotationDegrees += 2.0f;
    m_titleText->m_transform.scale = Vector2(fabs(sin(g_secondsInState * 2.0f)) + 0.5f);
}

//-----------------------------------------------------------------------------------
void TheGame::PressStart(NamedProperties&)
{
    SetGameState(PLAYER_JOIN);
    InitializePlayerJoinState();
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMainMenu() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::BLACK);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
void TheGame::EnqueueMinigames()
{
    for (int i = 0; i < m_numberOfMinigames; ++i)
    {
        m_queuedMinigameModes.push(new BattleRoyaleMinigameMode());
    }
}

//-----------------------------------------------------------------------------------
//PLAYER JOIN/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializePlayerJoinState()
{
    m_readyText[0] = new Sprite("ReadyText", PLAYER_LAYER);
    m_readyText[1] = new Sprite("ReadyText", PLAYER_LAYER);
    m_readyText[2] = new Sprite("ReadyText", PLAYER_LAYER);
    m_readyText[3] = new Sprite("ReadyText", PLAYER_LAYER);
    m_readyText[0]->m_position = Vector2(-1.0f, 1.0f);
    m_readyText[1]->m_position = Vector2(1.0f, 1.0f);
    m_readyText[2]->m_position = Vector2(-1.0f, -1.0f);
    m_readyText[3]->m_position = Vector2(1.0f, -1.0f);
    m_numberOfPlayers = 0;
    m_hasKeyboardPlayer = false;
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupPlayerJoinState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupPlayerJoinState(unsigned int)
{
    delete m_readyText[0];
    delete m_readyText[1];
    delete m_readyText[2];
    delete m_readyText[3];
    m_readyText[0] = nullptr;
    m_readyText[1] = nullptr;
    m_readyText[2] = nullptr;
    m_readyText[3] = nullptr;
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlayerJoin(float)
{
    //If someone presses their button a second time, we know all players are in and we're ready to start.
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept") || (m_numberOfPlayers == 4 && InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9)))
        {
            SetGameState(ASSEMBLY_GET_READY);
            InitializeAssemblyGetReadyState();
            return;
        }        
    }

    if (!m_hasKeyboardPlayer && m_numberOfPlayers < 4 && (InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9)))
    {
        m_readyText[m_numberOfPlayers]->m_tintColor = RGBA::GREEN;
        PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
        m_playerPilots.push_back(pilot);
        InitializeKeyMappingsForPlayer(pilot);
    }

    for (int i = 0; i < 4; ++i)
    {
        XInputController* controller = InputSystem::instance->m_controllers[i];
        if (controller->IsConnected() && m_numberOfPlayers < 4 && controller->JustPressed(XboxButton::START))
        {
            m_readyText[m_numberOfPlayers]->m_tintColor = RGBA::GREEN;
            PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
            pilot->m_controllerIndex = i;
            m_playerPilots.push_back(pilot);
            InitializeKeyMappingsForPlayer(pilot);
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderPlayerJoin() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBBLACK);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//ASSEMBLY GET READY/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyGetReadyState()
{
    m_getReadyBackground = new Sprite("AssemblyGetReady", PLAYER_LAYER);
    m_getReadyBackground->m_scale = Vector2(1.75f);
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyGetReadyState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyGetReadyState(unsigned int)
{
    delete m_getReadyBackground;
    m_getReadyBackground = nullptr;
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyGetReady(float)
{
    if (g_secondsInState < TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI)
    {
        return;
    }
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept") || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
        {
            SetGameState(ASSEMBLY_PLAYING);
            InitializeAssemblyPlayingState();
            return;
        }

    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyGetReady() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FOREST_GREEN);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//ASSEMBLY PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyPlayingState()
{
    m_gamePausedLabel = UISystem::instance->CreateWidget("Label");
    m_gamePausedLabel->SetProperty<std::string>("Name", "GamePausedLabel");
    m_gamePausedLabel->SetProperty<std::string>("Text", "Game Paused");
    m_gamePausedLabel->SetProperty("BackgroundColor", RGBA::CLEAR);
    m_gamePausedLabel->SetProperty("BorderWidth", 0.0f);
    m_gamePausedLabel->SetProperty("TextSize", 7.0f);
    m_gamePausedLabel->SetProperty("Offset", Vector2(1600.0f/4.0f, 900.0f/2.0f));
    UISystem::instance->AddWidget(m_gamePausedLabel);
    m_gamePausedLabel->SetHidden();

    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-20.0f, -20.0f), Vector2(20.0f, 20.0f)));
    m_currentGameMode = static_cast<GameMode*>(new AssemblyMode());
    m_currentGameMode->Initialize();
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());

    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyPlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyPlayingState(unsigned int)
{
    UISystem::instance->DeleteWidget(m_gamePausedLabel);
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyPlaying(float deltaSeconds)
{
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.FindInputValue("Pause")->WasJustReleased())
        {
            g_isGamePaused = !g_isGamePaused;
            if (g_isGamePaused)
            {
                SpriteGameRenderer::instance->AddEffectToLayer(m_pauseFBOEffect, FULL_SCREEN_EFFECT_LAYER);
                m_gamePausedLabel->SetVisible();
            }
            else
            {
                SpriteGameRenderer::instance->RemoveEffectFromLayer(m_pauseFBOEffect, FULL_SCREEN_EFFECT_LAYER);
                m_gamePausedLabel->SetHidden();
            }
            break;
        }
    }
    if (g_isGamePaused)
    {
        return;
    }

    m_currentGameMode->Update(deltaSeconds);
    if (!m_currentGameMode->m_isPlaying || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
    {
        SetGameState(ASSEMBLY_RESULTS);
        TheGame::instance->InitializeAssemblyResultsState();
        m_currentGameMode->CleanUp();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyPlaying() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FEEDFACE);
    SpriteGameRenderer::instance->Render();
    if (g_isGamePaused)
    {

    }
}

//-----------------------------------------------------------------------------------
//ASSEMBLY RESULTS/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyResultsState()
{
    m_currentGameMode->SetBackground("AssemblyResults", Vector2(1.75f));
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyResultsState);
    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        PlayerShip* ship = TheGame::instance->m_players[i];
        ship->LockMovement();
        float xMultiplier = i % 2 == 0 ? -1.0f : 1.0f;
        float yMultiplier = i >= 2 ? -1.0f : 1.0f;
        ship->SetPosition(Vector2(3.0f * xMultiplier, 3.0f * yMultiplier));
    }
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyResultsState(unsigned int)
{
    EnqueueMinigames();
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->UnlockMovement();
        ship->m_isDead = false;
        ship->Heal(999999999.0f);
    }
    delete m_currentGameMode;
    m_currentGameMode = m_queuedMinigameModes.front();
    m_queuedMinigameModes.pop();
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
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
        SetGameState(MINIGAME_GET_READY);
        InitializeMinigameGetReadyState();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyResults() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBLIGHTGREEN);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//MINIGAME GET READY/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigameGetReadyState()
{
    m_getReadyBackground = new Sprite("BattleRoyaleGetReady", UI_LAYER);
    m_getReadyBackground->m_scale = Vector2(1.75f);
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigameGetReadyState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigameGetReadyState(unsigned int)
{
    delete m_getReadyBackground;
    m_getReadyBackground = nullptr;
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigameGetReady(float)
{
    if (g_secondsInState < TIME_BEFORE_PLAYERS_CAN_ADVANCE_UI)
    {
        return;
    }
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept") || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
        {
            SetGameState(MINIGAME_PLAYING);
            InitializeMinigamePlayingState();
            return;
        }
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMinigameGetReady() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::CORNFLOWER_BLUE);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//MINIGAME PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigamePlayingState()
{
    m_currentGameMode->Initialize();
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigamePlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigamePlayingState(unsigned int)
{
    SpriteGameRenderer::instance->CreateOrGetLayer(BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigamePlaying(float deltaSeconds)
{
    m_currentGameMode->Update(deltaSeconds);
    if (!m_currentGameMode->m_isPlaying || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9))
    {
        SetGameState(MINIGAME_RESULTS);
        TheGame::instance->InitializeMinigameResultsState();
        m_currentGameMode->CleanUp();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMinigamePlaying() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FEEDFACE);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//MINIGAME RESULTS/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigameResultsState()
{
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    m_currentGameMode->SetBackground("MinigameResults", Vector2(1.75f));
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigameResultsState);
    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        PlayerShip* ship = TheGame::instance->m_players[i];
        ship->LockMovement();
        float xMultiplier = i % 2 == 0 ? -1.0f : 1.0f;
        float yMultiplier = i >= 2 ? -1.0f : 1.0f;
        ship->SetPosition(Vector2(3.0f * xMultiplier, 3.0f * yMultiplier));
    }
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigameResultsState(unsigned int)
{
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        ship->UnlockMovement();
        ship->m_isDead = false;
        ship->Heal(999999999.0f);
    }
    delete m_currentGameMode;
    if (m_queuedMinigameModes.size() > 0)
    {
        m_currentGameMode = m_queuedMinigameModes.front();
        m_queuedMinigameModes.pop();
    }
    else
    {
        m_currentGameMode = nullptr;
    }
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
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
        if (m_queuedMinigameModes.size() > 0)
        {
            SetGameState(MINIGAME_GET_READY);
            TheGame::instance->InitializeMinigameGetReadyState();
        }
        else
        {
            SetGameState(GAME_RESULTS_SCREEN);
            TheGame::instance->InitializeGameOverState();
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
    m_gameOverText->m_scale = Vector2(10.0f, 10.0f);
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupGameOverState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupGameOverState(unsigned int)
{
    delete m_gameOverText;
    for (PlayerShip* ship : m_players)
    {
        delete ship;
    }
    m_players.clear();
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateGameOver(float )
{
    bool keyboardStart = InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::F9);
    bool controllerStart = InputSystem::instance->WasButtonJustPressed(XboxButton::START) || InputSystem::instance->WasButtonJustPressed(XboxButton::A);
    if (keyboardStart || controllerStart)
    {
        SetGameState(MAIN_MENU);
        TheGame::instance->InitializeMainMenuState();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderGameOver() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::DISEASED);
    SpriteGameRenderer::instance->Render();
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
        playerPilot->m_inputMap.MapInputValue("Accept", keyboard->FindValue(InputSystem::ExtraKeys::ENTER));
        playerPilot->m_inputMap.MapInputValue("Accept", keyboard->FindValue(' '));
        playerPilot->m_inputMap.MapInputValue("Respawn", keyboard->FindValue(' '));
        playerPilot->m_inputMap.MapInputValue("Respawn", keyboard->FindValue('R'));
        playerPilot->m_inputMap.MapInputValue("Pause", keyboard->FindValue('P'));
    }
    else
    {
        XInputDevice* controller = InputSystem::instance->m_xInputDevices[playerPilot->m_controllerIndex];
        //CONTROLLER INPUT
        playerPilot->m_inputMap.MapInputAxis("Up")->AddMapping(&controller->GetLeftStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("Right")->AddMapping(&controller->GetLeftStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootUp")->AddMapping(&controller->GetRightStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootRight")->AddMapping(&controller->GetRightStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputValue("Suicide", controller->FindButton(XboxButton::B));
        playerPilot->m_inputMap.MapInputValue("Shoot", ChordResolutionMode::RESOLVE_MAXS_ABSOLUTE)->m_deadzoneValue = XInputController::INNER_DEADZONE;
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightTrigger());
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightStickMagnitude());
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("Respawn", controller->FindButton(XboxButton::BACK));
        playerPilot->m_inputMap.MapInputValue("Pause", controller->FindButton(XboxButton::START));
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSprites()
{
    //Backgrounds
    ResourceDatabase::instance->RegisterSprite("Twah", "Data\\Images\\Twah.png");
    ResourceDatabase::instance->RegisterSprite("Cloudy", "Data\\Images\\Particles\\Cloudy_Thicc.png");
    ResourceDatabase::instance->RegisterSprite("DefaultBackground", "Data\\Images\\Backgrounds\\Nebula.jpg");
    ResourceDatabase::instance->RegisterSprite("Assembly", "Data\\Images\\Backgrounds\\Assembly.png");
    ResourceDatabase::instance->RegisterSprite("BattleBackground", "Data\\Images\\Backgrounds\\Orange-space.jpg");
    ResourceDatabase::instance->RegisterSprite("AssemblyResults", "Data\\Images\\assemblyResultsMockup.png");
    ResourceDatabase::instance->RegisterSprite("MinigameResults", "Data\\Images\\minigameResultsMockup.png");
    ResourceDatabase::instance->RegisterSprite("AssemblyGetReady", "Data\\Images\\assemblyGetReadyMockup.png");
    ResourceDatabase::instance->RegisterSprite("BattleRoyaleGetReady", "Data\\Images\\battleRoyaleGetReadyMockup.png");
    ResourceDatabase::instance->RegisterSprite("ReadyText", "Data\\Images\\ready.png");

    //Entities
    ResourceDatabase::instance->RegisterSprite("Laser", "Data\\Images\\Lasers\\laserColorless10.png");
    ResourceDatabase::instance->RegisterSprite("MuzzleFlash", "Data\\Images\\Lasers\\muzzleFlash.png");
    ResourceDatabase::instance->RegisterSprite("Pico", "Data\\Images\\Pico.png");
    ResourceDatabase::instance->RegisterSprite("PlayerShip", "Data\\Images\\garbageRecolorableShip.png");
    ResourceDatabase::instance->RegisterSprite("Shield", "Data\\Images\\Shield.png");
    ResourceDatabase::instance->RegisterSprite("TitleText", "Data\\Images\\Title.png");
    ResourceDatabase::instance->RegisterSprite("GameOverText", "Data\\Images\\GameOver.png");
    ResourceDatabase::instance->RegisterSprite("ItemBox", "Data\\Images\\ItemBox.png");
    ResourceDatabase::instance->RegisterSprite("GreenEnemy", "Data\\Images\\Enemies\\enemyGreen1.png");
    ResourceDatabase::instance->RegisterSprite("Asteroid", "Data\\Images\\Props\\asteroid01.png");
    ResourceDatabase::instance->RegisterSprite("Invalid", "Data\\Images\\invalidSpriteResource.png");

    //Pickups
    ResourceDatabase::instance->RegisterSprite("TopSpeed", "Data\\Images\\Pickups\\speed.png");
    ResourceDatabase::instance->RegisterSprite("Acceleration", "Data\\Images\\Pickups\\acceleration.png");
    ResourceDatabase::instance->RegisterSprite("Handling", "Data\\Images\\Pickups\\handling.png");
    ResourceDatabase::instance->RegisterSprite("Braking", "Data\\Images\\Pickups\\braking.png");
    ResourceDatabase::instance->RegisterSprite("Damage", "Data\\Images\\Pickups\\damage.png");
    ResourceDatabase::instance->RegisterSprite("ShieldDisruption", "Data\\Images\\Pickups\\shieldDisruption.png");
    ResourceDatabase::instance->RegisterSprite("ShieldPenetration", "Data\\Images\\Pickups\\shieldPenetration.png");
    ResourceDatabase::instance->RegisterSprite("RateOfFire", "Data\\Images\\Pickups\\fireRate.png");
    ResourceDatabase::instance->RegisterSprite("Hp", "Data\\Images\\Pickups\\hp.png");
    ResourceDatabase::instance->RegisterSprite("ShieldCapacity", "Data\\Images\\Pickups\\shieldCapacity.png");
    ResourceDatabase::instance->RegisterSprite("ShieldRegen", "Data\\Images\\Pickups\\shieldRegen.png");
    ResourceDatabase::instance->RegisterSprite("ShotDeflection", "Data\\Images\\Pickups\\shotDeflection.png");

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
    ResourceDatabase::instance->RegisterSprite("White4Star", "Data\\Images\\Particles\\particleWhite_7.png");
    ResourceDatabase::instance->RegisterSprite("Yellow4Star", "Data\\Images\\Particles\\particleYellow_7.png");
    ResourceDatabase::instance->RegisterSprite("Yellow5Star", "Data\\Images\\Particles\\particleYellow_3.png");
    ResourceDatabase::instance->RegisterSprite("YellowCircle", "Data\\Images\\Particles\\particleYellow_8.png");
    ResourceDatabase::instance->RegisterSprite("YellowBeam", "Data\\Images\\Particles\\particleYellow_9.png");
    ResourceDatabase::instance->EditSpriteResource("YellowBeam")->m_pivotPoint.y = 0.0f;

}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSpriteAnimations()
{
    SpriteAnimationResource* shieldAnimation = ResourceDatabase::instance->RegisterSpriteAnimation("Shield", SpriteAnimationLoopMode::LOOP);
    shieldAnimation->AddFrame("Shield", 0.5f);
    shieldAnimation->AddFrame("Yellow4Star", 0.7f);
    shieldAnimation->AddFrame("Acceleration", 0.5f);
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterParticleEffects()
{
    const float DEATH_ANIMATION_LENGTH = 1.5f;
    const float POWER_UP_PICKUP_ANIMATION_LENGTH = 0.15f;
    const float MUZZLE_FLASH_ANIMATION_LENGTH = 0.01f;
    const float CRATE_DESTRUCTION_ANIMATION_LENGTH = 0.6f; 
    const float COLLISION_ANIMATION_LENGTH = 0.3f;

    //EMITTERS/////////////////////////////////////////////////////////////////////
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
    yellowExplosionOrb->m_properties.Set<std::string>(PROPERTY_NAME, "Yellow Explosion Orb");
    yellowExplosionOrb->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    yellowExplosionOrb->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    yellowExplosionOrb->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, DEATH_ANIMATION_LENGTH);
    yellowExplosionOrb->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    yellowExplosionOrb->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(1.3f));

    ParticleEmitterDefinition* yellowBeams = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("YellowBeam"));
    yellowBeams->m_properties.Set<std::string>(PROPERTY_NAME, "Yellow Beams");
    yellowBeams->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    yellowBeams->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, Range<unsigned int>(5, 10));
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Range<Vector2>(Vector2(0.2f), Vector2(0.4f)));
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    yellowBeams->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, DEATH_ANIMATION_LENGTH);
    yellowBeams->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 0.0f);
    yellowBeams->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.0f, 2.0f));
    yellowBeams->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));

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

    ParticleEmitterDefinition* shipTrail = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("BlackSmoke"));
    shipTrail->m_properties.Set<std::string>(PROPERTY_NAME, "Ship Trail");
    shipTrail->m_properties.Set<bool>(PROPERTY_FADEOUT_ENABLED, true);
    shipTrail->m_properties.Set<Range<unsigned int>>(PROPERTY_INITIAL_NUM_PARTICLES, 1);
    shipTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_SCALE, Vector2(0.2f));
    shipTrail->m_properties.Set<Range<Vector2>>(PROPERTY_INITIAL_VELOCITY, Vector2::ZERO);
    shipTrail->m_properties.Set<Range<float>>(PROPERTY_PARTICLE_LIFETIME, 1.5f);
    shipTrail->m_properties.Set<float>(PROPERTY_PARTICLES_PER_SECOND, 5.0f);
    shipTrail->m_properties.Set<Range<float>>(PROPERTY_MAX_EMITTER_LIFETIME, FLT_MAX);
    shipTrail->m_properties.Set<Range<Vector2>>(PROPERTY_DELTA_SCALE_PER_SECOND, Vector2(0.3f));
    crateDestroyed->m_properties.Set<Range<float>>(PROPERTY_INITIAL_ROTATION_DEGREES, Range<float>(0.0f, 360.0f));

    ParticleEmitterDefinition* collisionParticle = new ParticleEmitterDefinition(ResourceDatabase::instance->GetSpriteResource("Cloudy"));
    collisionParticle->m_properties.Set<std::string>(PROPERTY_NAME, "Collision");
    //crateDestroyed->m_properties.Set<float>("Gravity Scale", 1.0f);
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
    titleScreenParticle->m_properties.Set<float>("Gravity Scale", -1.0f);
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

    //SYSTEMS/////////////////////////////////////////////////////////////////////
    ParticleSystemDefinition* deathParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Death", ONE_SHOT);
    deathParticleSystem->AddEmitter(yellowStars);
    deathParticleSystem->AddEmitter(yellowExplosionOrb);
    deathParticleSystem->AddEmitter(yellowBeams);

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

    ParticleSystemDefinition* titleScreenParticleSystem = ResourceDatabase::instance->RegisterParticleSystem("Title", LOOPING);
    titleScreenParticleSystem->AddEmitter(titleScreenParticle);
}
