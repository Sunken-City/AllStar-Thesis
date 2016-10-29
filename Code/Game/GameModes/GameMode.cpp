#include "Game/GameModes/GameMode.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"

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
