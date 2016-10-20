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
#include "Game/Entities/Player.hpp"
#include "Game/Entities/Projectile.hpp"
#include "Game/Entities/Ship.hpp"
#include "Entities/ItemBox.hpp"
#include "Entities/Grunt.hpp"
#include "Entities/Pickup.hpp"
#include "Engine/Input/InputDevices.hpp"

TheGame* TheGame::instance = nullptr;

Sprite* testBackground = nullptr;
Sprite* titleText = nullptr;
Sprite* gameOverText = nullptr;
float m_timeSinceLastSpawn = 0.0f;
const float TIME_PER_SPAWN = 1.0f;

//-----------------------------------------------------------------------------------
TheGame::TheGame()
{
    ResourceDatabase::instance = new ResourceDatabase();
    RegisterSprites();
    SetGameState(GameState::MAIN_MENU);
    InitializeKeyMappings();
    InitializeMainMenuState();
}

//-----------------------------------------------------------------------------------
TheGame::~TheGame()
{
    SetGameState(GameState::SHUTDOWN);
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
    if (m_gameplayMapping.WasJustPressed("Accept"))
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
    testBackground = new Sprite("Nebula", BACKGROUND_LAYER);
    testBackground->m_scale = Vector2(10.0f, 10.0f);
    Player* player1 = new Player();
    m_entities.push_back(player1);
    m_players.push_back(player1);
    ItemBox* box1 = new ItemBox(Vector2(2.0f));
    m_entities.push_back(box1);
    ItemBox* box2 = new ItemBox(Vector2(1.0f));
    m_entities.push_back(box2);
    Grunt* g1 = new Grunt(Vector2(-2.0f));
    m_entities.push_back(g1);
    Grunt* g2 = new Grunt(Vector2(-1.0f));
    m_entities.push_back(g2);
    SpriteGameRenderer::instance->SetWorldBounds(testBackground->GetBounds());
    OnStateSwitch.RegisterMethod(this, &TheGame::CleanupPlayingState);
}

//-----------------------------------------------------------------------------------
void TheGame::CleanupPlayingState(unsigned int)
{
    for (Entity* ent : m_entities)
    {
        delete ent;
    }
    m_entities.clear();
    m_players.clear();
    delete testBackground;
    SpriteGameRenderer::instance->SetCameraPosition(Vector2::ZERO);
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlaying(float deltaSeconds)
{
    m_timeSinceLastSpawn += deltaSeconds;
    if (m_timeSinceLastSpawn > TIME_PER_SPAWN)
    {
        m_entities.push_back(new ItemBox(testBackground->GetBounds().GetRandomPointInside()));
        m_entities.push_back(new Grunt(testBackground->GetBounds().GetRandomPointInside()));
        m_timeSinceLastSpawn = 0.0f;
    }
    for (Entity* ent : m_entities)
    {
        ent->Update(deltaSeconds);
        for (Entity* other : m_entities)
        {
            if ((ent != other) && (ent->IsCollidingWith(other)))
            {
                ent->ResolveCollision(other);
            }
        }
    }
    for (Entity* ent : m_newEntities)
    {
        m_entities.push_back(ent);
    }
    m_newEntities.clear();
    for (auto iter = m_entities.begin(); iter != m_entities.end(); ++iter)
    {
        Entity* gameObject = *iter;
        if (gameObject->m_isDead)
        {
            delete gameObject;
            iter = m_entities.erase(iter);
        }
        if (iter == m_entities.end())
        {
            break;
        }
    }
    if (!m_players[0] || m_players[0]->m_isDead)
    {
        SetGameState(GAME_OVER);
        InitializeGameOverState();
    }
    else
    {
        SpriteGameRenderer::instance->SetCameraPosition(m_players[0]->m_sprite->m_position);
    }
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
    m_newEntities.push_back(new Projectile(creator));
}

//-----------------------------------------------------------------------------------
void TheGame::SpawnPickup(const Vector2& spawnPosition)
{
    m_newEntities.push_back(new Pickup(spawnPosition));
}

//-----------------------------------------------------------------------------------
void TheGame::InitializeKeyMappings()
{
    KeyboardInputDevice* keyboard = InputSystem::instance->m_keyboardDevice;
    MouseInputDevice* mouse = InputSystem::instance->m_mouseDevice;
    m_gameplayMapping.AddInputAxis("Up", keyboard->FindValue('W'), keyboard->FindValue('S'));
    m_gameplayMapping.AddInputAxis("Right", keyboard->FindValue('D'), keyboard->FindValue('A'));
    m_gameplayMapping.AddInputAxis("ShootRight", mouse->m_deltaPosition.m_xPos, mouse->m_deltaPosition.m_xNeg);
    m_gameplayMapping.AddInputAxis("ShootUp", mouse->m_deltaPosition.m_yPos, mouse->m_deltaPosition.m_yNeg);
    m_gameplayMapping.AddInputValue("Shoot", keyboard->FindValue(' '));
    m_gameplayMapping.AddInputValue("Shoot", mouse->FindButtonValue(InputSystem::MouseButtons::LEFT_MOUSE_BUTTON));
    m_gameplayMapping.AddInputValue("Accept", keyboard->FindValue(InputSystem::ExtraKeys::ENTER));
    m_gameplayMapping.AddInputValue("Accept", keyboard->FindValue(' '));
}

//-----------------------------------------------------------------------------------
void TheGame::RegisterSprites()
{
    ResourceDatabase::instance->RegisterSprite("Laser", "Data\\Images\\Lasers\\laserGreen10.png");
    ResourceDatabase::instance->RegisterSprite("Pico", "Data\\Images\\Pico.png");
    ResourceDatabase::instance->RegisterSprite("PlayerShip", "Data\\Images\\spaceShips_005.png");
    ResourceDatabase::instance->RegisterSprite("Twah", "Data\\Images\\Twah.png");
    ResourceDatabase::instance->RegisterSprite("Nebula", "Data\\Images\\Nebula.jpg");
    ResourceDatabase::instance->RegisterSprite("TitleText", "Data\\Images\\Title.png");
    ResourceDatabase::instance->RegisterSprite("GameOverText", "Data\\Images\\GameOver.png");
    ResourceDatabase::instance->RegisterSprite("ItemBox", "Data\\Images\\ItemBox.png");
    ResourceDatabase::instance->RegisterSprite("GreenEnemy", "Data\\Images\\Enemies\\enemyGreen1.png");
    ResourceDatabase::instance->RegisterSprite("Power", "Data\\Images\\Pickups\\power.png");
    ResourceDatabase::instance->RegisterSprite("FireRate", "Data\\Images\\Pickups\\fireRate.png");
    ResourceDatabase::instance->RegisterSprite("Hp", "Data\\Images\\Pickups\\hp.png");
    ResourceDatabase::instance->RegisterSprite("Speed", "Data\\Images\\Pickups\\speed.png");
    ResourceDatabase::instance->RegisterSprite("Defence", "Data\\Images\\Pickups\\defence.png");
    ResourceDatabase::instance->RegisterSprite("Invalid", "Data\\Images\\invalidSpriteResource.png");
}