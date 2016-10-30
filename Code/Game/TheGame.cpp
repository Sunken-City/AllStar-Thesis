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
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Projectile.hpp"
#include "Game/Entities/Ship.hpp"
#include "Entities/ItemCrate.hpp"
#include "Entities/Grunt.hpp"
#include "Entities/Pickup.hpp"
#include "Engine/Input/InputDevices/KeyboardInputDevice.hpp"
#include "Engine/Input/InputDevices/MouseInputDevice.hpp"
#include "Pilots/PlayerPilot.hpp"
#include "Engine/Input/InputDevices/XInputDevice.hpp"
#include "Engine/Input/InputValues.hpp"
#include "GameModes/AssemblyMode.hpp"

TheGame* TheGame::instance = nullptr;

Sprite* testBackground = nullptr;
Sprite* titleText = nullptr;
Sprite* gameOverText = nullptr;

//-----------------------------------------------------------------------------------
TheGame::TheGame()
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
    for (PlayerPilot* pilot : m_playerPilots)
    {
        delete pilot;
    }
    m_playerPilots.clear();
    delete ResourceDatabase::instance;
    ResourceDatabase::instance = nullptr;
}

//-----------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds)
{
    SpriteGameRenderer::instance->Update(deltaSeconds);
    if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::TILDE))
    {
        Console::instance->ToggleConsole();
    }
    if (Console::instance->IsActive())
    {
        return;
    }

#pragma todo("Reenable menu navigation once we have a more solid game flow")
    if (InputSystem::instance->WasKeyJustPressed(' '))
    {
        switch (GetGameState())
        {
        case MAIN_MENU:
            SetGameState(PLAYING);
            InitializePlayingState();
            break;
        case PLAYING:
            //SetGameState(GAME_OVER);
            //InitializeGameOverState();
            break;
        case GAME_OVER:
            //SetGameState(MAIN_MENU);
            //InitializeMainMenuState();
            break;
        default:
            break;
        }
    }
    else if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::BACKSPACE))
    {
        switch (GetGameState())
        {
        case GAME_OVER:
            //SetGameState(PLAYING);
            //InitializePlayingState();
            break;
        case PLAYING:
            //SetGameState(MAIN_MENU);
            //InitializeMainMenuState();
            break;
        default:
            break;
        }
    }

    switch (GetGameState())
    {
    case MAIN_MENU:
        UpdateMainMenu(deltaSeconds);
        break;
    case STARTUP:
        break;
    case PLAYING:
        UpdatePlaying(deltaSeconds);
        break;
    case PAUSED:
        //TODO: This will clean up all the game objects because of our callbacks, be careful here.
        break;
    case GAME_OVER:
        UpdateGameOver(deltaSeconds);
        break;
    case SHUTDOWN:
        break;
    case NUM_STATES:
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
    case STARTUP:
        break;
    case PLAYING:
        RenderPlaying();
        break;
    case PAUSED:
        break;
    case GAME_OVER:
        RenderGameOver();
        break;
    case SHUTDOWN:
        break;
    case NUM_STATES:
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
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMainMenu(float deltaSeconds)
{
    UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMainMenu() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::CERULEAN);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
//PLAYING/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void TheGame::InitializePlayingState()
{
    m_playerPilots.push_back(new PlayerPilot());
    InitializeKeyMappingsForPlayer(m_playerPilots[0]);

    for (int i = 1; i < 4; ++i)
    {
        if (InputSystem::instance->m_controllers[i - 1]->IsConnected())
        {
            m_playerPilots.push_back(new PlayerPilot(i));
            InitializeKeyMappingsForPlayer(m_playerPilots[i]);
        }
    }

    m_currentGameMode = static_cast<GameMode*>(new AssemblyMode());
    SpriteGameRenderer::instance->SetSplitscreen(m_playerPilots.size());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupPlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupPlayingState(unsigned int)
{
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
    delete m_currentGameMode;
    SpriteGameRenderer::instance->SetSplitscreen(1);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlaying(float deltaSeconds)
{
    m_currentGameMode->Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void TheGame::RenderPlaying() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::FEEDFACE);
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
    //TODO: SpriteGameRenderer::instance->AddEffectToLayer()
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupGameOverState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupGameOverState(unsigned int)
{
    delete gameOverText;
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateGameOver(float deltaSeconds)
{
    UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void TheGame::RenderGameOver() const
{
    SpriteGameRenderer::instance->SetClearColor(RGBA::DISEASED);
    SpriteGameRenderer::instance->Render();
}

//-----------------------------------------------------------------------------------
void TheGame::SpawnBullet(Ship* creator)
{
    m_currentGameMode->m_newEntities.push_back(new Projectile(creator));
}

//-----------------------------------------------------------------------------------
void TheGame::SpawnPickup(Item* item, const Vector2& spawnPosition)
{
    ASSERT_OR_DIE(item, "Item was null when attempting to spawn pickup");
    m_currentGameMode->m_newEntities.push_back(new Pickup(item, spawnPosition));
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
        playerPilot->m_inputMap.MapInputValue("Shoot", &controller->GetRightStick()->m_xAxis);
        playerPilot->m_inputMap.MapInputValue("Shoot", &controller->GetRightStick()->m_yAxis);
        playerPilot->m_inputMap.MapInputValue("Shoot", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::A));
        playerPilot->m_inputMap.MapInputValue("Accept", controller->FindButton(XboxButton::START));
        playerPilot->m_inputMap.MapInputValue("Respawn", controller->FindButton(XboxButton::START));
    }
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSprites()
{
    ResourceDatabase::instance->RegisterSprite("Laser", "Data\\Images\\Lasers\\laserGreen10.png");
    ResourceDatabase::instance->RegisterSprite("Pico", "Data\\Images\\Pico.png");
    ResourceDatabase::instance->RegisterSprite("PlayerShip", "Data\\Images\\spaceShips_005.png");
    ResourceDatabase::instance->RegisterSprite("Twah", "Data\\Images\\Twah.png");
    ResourceDatabase::instance->RegisterSprite("DefaultBackground", "Data\\Images\\Nebula.jpg");
    ResourceDatabase::instance->RegisterSprite("TitleText", "Data\\Images\\Title.png");
    ResourceDatabase::instance->RegisterSprite("GameOverText", "Data\\Images\\GameOver.png");
    ResourceDatabase::instance->RegisterSprite("ItemBox", "Data\\Images\\ItemBox.png");
    ResourceDatabase::instance->RegisterSprite("GreenEnemy", "Data\\Images\\Enemies\\enemyGreen1.png");
    ResourceDatabase::instance->RegisterSprite("Invalid", "Data\\Images\\invalidSpriteResource.png");

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