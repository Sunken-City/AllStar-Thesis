#include "Game/Entities/MinigameEntities/OuroborosCoin.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "../PlayerShip.hpp"
#include "Game/GameModes/Minigames/OuroborosMinigameMode.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../TextSplash.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/Pilots/PlayerPilot.hpp"

//-----------------------------------------------------------------------------------
OuroborosCoin::OuroborosCoin(PlayerShip* owner, const Vector2& position, int value /*= 0*/)
    : Coin(position, value)
    , m_owner(owner)
{
    m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("OuroborosCoin");
    m_sprite->m_material = owner->m_sprite->m_material;
    m_velocity = Vector2::ZERO;
    SetPosition(position);

    if (value > OUROBOROS_VALUE)
    {
        m_scale = Vector2(2.0f);
    }

    uchar visibilityFilterForPlayer = (uchar)SpriteGameRenderer::instance->GetVisibilityFilterForPlayerNumber(((PlayerPilot*)owner->m_pilot)->m_playerNumber);
    uchar visibilityFilterForEveryoneButPlayer = (static_cast<uchar>(SpriteGameRenderer::PlayerVisibility::ALL) ^ visibilityFilterForPlayer);
    m_sprite->m_viewableBy = visibilityFilterForEveryoneButPlayer;
}

//-----------------------------------------------------------------------------------
OuroborosCoin::~OuroborosCoin()
{
}

//-----------------------------------------------------------------------------------
void OuroborosCoin::Update(float deltaSeconds)
{
    Coin::Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void OuroborosCoin::Render() const
{
    Coin::Render();
}

//-----------------------------------------------------------------------------------
void OuroborosCoin::ResolveCollision(Entity* otherEntity)
{
    if ((PlayerShip*)otherEntity == m_owner)
    {
        return;
    }
    else
    {
        Coin::ResolveCollision(otherEntity);
    }
}
