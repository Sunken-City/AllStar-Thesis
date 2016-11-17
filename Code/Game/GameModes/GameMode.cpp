#include "Game/GameModes/GameMode.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/StateMachine.hpp"

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(arenaBackgroundImage, TheGame::BACKGROUND_LAYER)
{
    m_arenaBackground.m_scale = Vector2(10.0f, 10.0f);
    SpriteGameRenderer::instance->SetWorldBounds(m_arenaBackground.GetBounds());

}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{

}

//-----------------------------------------------------------------------------------
void GameMode::Update(float deltaSeconds)
{
    if (m_isPlaying)
    {
        m_timerSecondsElapsed += deltaSeconds;
    }
    if (m_timerSecondsElapsed >= m_gameLengthSeconds)
    {
        m_isPlaying = false; 
    }
}

//-----------------------------------------------------------------------------------
Vector2 GameMode::GetRandomLocationInArena()
{
    return m_arenaBackground.GetBounds().GetRandomPointInside();
}

//-----------------------------------------------------------------------------------
Vector2 GameMode::GetRandomPlayerSpawnPoint()
{
    if (m_playerSpawnPoints.size() > 0)
    {
        int randomPoint = MathUtils::GetRandomIntFromZeroTo(m_playerSpawnPoints.size());
        return m_playerSpawnPoints[randomPoint];
    }
    else
    {
        return GetRandomLocationInArena();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::AddPlayerSpawnPoint(const Vector2& newSpawnPoint)
{
    m_playerSpawnPoints.push_back(newSpawnPoint);
}

//-----------------------------------------------------------------------------------
AABB2 GameMode::GetArenaBounds()
{
    return m_arenaBackground.GetBounds();
}
