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

TheGame* TheGame::instance = nullptr;

Sprite* testBackground = nullptr;
Sprite* titleText = nullptr;
Sprite* gameOverText = nullptr;
Sprite* readyText[4];

//-----------------------------------------------------------------------------------
TheGame::TheGame()
    : m_currentGameMode(nullptr)
    , SFX_UI_ADVANCE(AudioSystem::instance->CreateOrGetSound("Data/SFX/UI/UI_Select_01.wav"))
{
    ResourceDatabase::instance = new ResourceDatabase();
    RegisterSprites();
    SetGameState(GameState::MAIN_MENU);
    InitializeMainMenuState();
}


//-----------------------------------------------------------------------------------
TheGame::~TheGame()
{
    SetGameState(GameState::SHUTDOWN);

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
        //UpdateAssemblyGetReady(deltaSeconds);
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
        //UpdateMinigameGetReady(deltaSeconds);
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
        //RenderAssemblyGetReady();
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
        //RenderMinigameGetReady();
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
    titleText = new Sprite("TitleText", PLAYER_LAYER);
    titleText->m_scale = Vector2(10.0f, 10.0f);
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMainMenuState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMainMenuState(unsigned int)
{
    delete titleText;
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMainMenu(float deltaSeconds)
{
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) || InputSystem::instance->WasKeyJustPressed(' '))
    {
        SetGameState(PLAYER_JOIN);
        InitializePlayerJoinState();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMainMenu() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::CERULEAN);
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
    readyText[0] = new Sprite("ReadyText", PLAYER_LAYER);
    readyText[1] = new Sprite("ReadyText", PLAYER_LAYER);
    readyText[2] = new Sprite("ReadyText", PLAYER_LAYER);
    readyText[3] = new Sprite("ReadyText", PLAYER_LAYER);
    readyText[0]->m_position = Vector2(-1.0f, 1.0f);
    readyText[1]->m_position = Vector2(1.0f, 1.0f);
    readyText[2]->m_position = Vector2(-1.0f, -1.0f);
    readyText[3]->m_position = Vector2(1.0f, -1.0f);
    m_numberOfPlayers = 0;
    m_hasKeyboardPlayer = false;
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupPlayerJoinState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupPlayerJoinState(unsigned int)
{
    delete readyText[0];
    delete readyText[1];
    delete readyText[2];
    delete readyText[3];
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlayerJoin(float deltaSeconds)
{
    for (PlayerPilot* pilot : m_playerPilots)
    {
        if (pilot->m_inputMap.WasJustPressed("Accept"))
        {
            SetGameState(ASSEMBLY_PLAYING);
            InitializeAssemblyState();
        }
        
    }
    if (!m_hasKeyboardPlayer && InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER) && m_numberOfPlayers < 4)
    {
        readyText[m_numberOfPlayers]->m_tintColor = RGBA::GREEN;
        PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
        m_playerPilots.push_back(pilot);
        InitializeKeyMappingsForPlayer(pilot);
    }

    for (int i = 0; i < 4; ++i)
    {
        XInputController* controller = InputSystem::instance->m_controllers[i];
        if (controller->IsConnected() && controller->JustPressed(XboxButton::START) && m_numberOfPlayers < 4)
        {
            readyText[m_numberOfPlayers]->m_tintColor = RGBA::GREEN;
            PlayerPilot* pilot = new PlayerPilot(m_numberOfPlayers++);
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
//PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyState()
{
    m_currentGameMode = static_cast<GameMode*>(new AssemblyMode());
    m_currentGameMode->Initialize();
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupAssemblyState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupAssemblyState(unsigned int)
{
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateAssemblyPlaying(float deltaSeconds)
{
    m_currentGameMode->Update(deltaSeconds);
    if (!m_currentGameMode->m_isPlaying)
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
}

//-----------------------------------------------------------------------------------
//ASSEMBLY RESULTS/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeAssemblyResultsState()
{
    m_currentGameMode->SetBackground("AssemblyResults", Vector2(1.75f));
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
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER))
    {
        SetGameState(MINIGAME_PLAYING);
        TheGame::instance->InitializeMinigameState();
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RenderAssemblyResults() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::GBLIGHTGREEN);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//MINIGAME PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializeMinigameState()
{
    m_currentGameMode->Initialize();
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupMinigameState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupMinigameState(unsigned int)
{
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    SpriteGameRenderer::instance->SetSplitscreen(1);
    AudioSystem::instance->PlaySound(SFX_UI_ADVANCE);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMinigamePlaying(float deltaSeconds)
{
    m_currentGameMode->Update(deltaSeconds);
    if (!m_currentGameMode->m_isPlaying)
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
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER))
    {
        if (m_queuedMinigameModes.size() > 0)
        {
            SetGameState(MINIGAME_PLAYING);
            TheGame::instance->InitializeMinigameState();
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
    gameOverText = new Sprite("GameOverText", PLAYER_LAYER);
    gameOverText->m_scale = Vector2(10.0f, 10.0f);
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupGameOverState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupGameOverState(unsigned int)
{
    delete gameOverText;
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
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER))
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
    if (playerPilot->m_playerNumber == 0)
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
    }
    else
    {
        XInputDevice* controller = InputSystem::instance->m_xInputDevices[playerPilot->m_playerNumber - 1];
        //CONTROLLER INPUT
        playerPilot->m_inputMap.MapInputAxis("Up")->AddMapping(&controller->GetLeftStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("Right")->AddMapping(&controller->GetLeftStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootUp")->AddMapping(&controller->GetRightStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputAxis("ShootRight")->AddMapping(&controller->GetRightStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputValue("Suicide", controller->FindButton(XboxButton::BACK));
        playerPilot->m_inputMap.MapInputValue("Shoot", ChordResolutionMode::RESOLVE_MAXS_ABSOLUTE)->m_deadzoneValue = XInputController::INNER_DEADZONE;
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightTrigger());
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->GetRightStickMagnitude());
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("Respawn", controller->FindButton(XboxButton::START));
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSprites()
{
    //Backgrounds
    ResourceDatabase::instance->RegisterSprite("Twah", "Data\\Images\\Twah.png");
    ResourceDatabase::instance->RegisterSprite("DefaultBackground", "Data\\Images\\Nebula.jpg");
    ResourceDatabase::instance->RegisterSprite("BattleBackground", "Data\\Images\\Orange-space.jpg");
    ResourceDatabase::instance->RegisterSprite("AssemblyResults", "Data\\Images\\assemblyResultsMockup.png");
    ResourceDatabase::instance->RegisterSprite("MinigameResults", "Data\\Images\\minigameResultsMockup.png");
    ResourceDatabase::instance->RegisterSprite("AssemblyGetReady", "Data\\Images\\assemblyGetReadyMockup.png");
    ResourceDatabase::instance->RegisterSprite("BattleRoyaleGetReady", "Data\\Images\\battleRoyaleGetReadyMockup.png");
    ResourceDatabase::instance->RegisterSprite("ReadyText", "Data\\Images\\ready.png");

    //Entities
    ResourceDatabase::instance->RegisterSprite("Laser", "Data\\Images\\Lasers\\laserGreen10.png");
    ResourceDatabase::instance->RegisterSprite("Pico", "Data\\Images\\Pico.png");
    ResourceDatabase::instance->RegisterSprite("PlayerShip", "Data\\Images\\garbageRecolorableShip.png");
    ResourceDatabase::instance->RegisterSprite("TitleText", "Data\\Images\\Title.png");
    ResourceDatabase::instance->RegisterSprite("GameOverText", "Data\\Images\\GameOver.png");
    ResourceDatabase::instance->RegisterSprite("ItemBox", "Data\\Images\\ItemBox.png");
    ResourceDatabase::instance->RegisterSprite("GreenEnemy", "Data\\Images\\Enemies\\enemyGreen1.png");
    ResourceDatabase::instance->RegisterSprite("Asteroid", "Data\\Images\\Props\\asteroid01.png");
    ResourceDatabase::instance->RegisterSprite("Invalid", "Data\\Images\\invalidSpriteResource.png");

    //Pickups
    ResourceDatabase::instance->RegisterSprite("TopSpeed", "Data\\Images\\Pickups\\speed.png");
    ResourceDatabase::instance->RegisterSprite("Acceleration", "Data\\Images\\Pickups\\boost.png");
    ResourceDatabase::instance->RegisterSprite("Handling", "Data\\Images\\Pickups\\handling.png");
    ResourceDatabase::instance->RegisterSprite("Braking", "Data\\Images\\Pickups\\braking.png");
    ResourceDatabase::instance->RegisterSprite("Damage", "Data\\Images\\Pickups\\power.png");
    ResourceDatabase::instance->RegisterSprite("ShieldDisruption", "Data\\Images\\invalidSpriteResource.png");
    ResourceDatabase::instance->RegisterSprite("ShieldPenetration", "Data\\Images\\invalidSpriteResource.png");
    ResourceDatabase::instance->RegisterSprite("RateOfFire", "Data\\Images\\Pickups\\fireRate.png");
    ResourceDatabase::instance->RegisterSprite("Hp", "Data\\Images\\Pickups\\hp.png");
    ResourceDatabase::instance->RegisterSprite("ShieldCapacity", "Data\\Images\\invalidSpriteResource.png");
    ResourceDatabase::instance->RegisterSprite("ShieldRegen", "Data\\Images\\invalidSpriteResource.png");
    ResourceDatabase::instance->RegisterSprite("ShotDeflection", "Data\\Images\\invalidSpriteResource.png");
}