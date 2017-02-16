#include "Game/Entities/Props/Wormhole.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"

const float Wormhole::MAX_ANGULAR_VELOCITY = 20.0f;

//-----------------------------------------------------------------------------------
Wormhole::Wormhole(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Wormhole", TheGame::GEOMETRY_LAYER);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
}

//-----------------------------------------------------------------------------------
Wormhole::~Wormhole()
{

}

//-----------------------------------------------------------------------------------
void Wormhole::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    float newRotationDegrees = m_sprite->m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_sprite->m_transform.SetRotationDegrees(newRotationDegrees);
}

//-----------------------------------------------------------------------------------
void Wormhole::ResolveCollision(Entity* otherEntity)
{
    Vector2 dispFromOtherToCenter = m_transform.GetWorldPosition() - otherEntity->m_transform.GetWorldPosition();
    Vector2 normDirectionTowardsCenter = dispFromOtherToCenter.GetNorm();
    otherEntity->ApplyImpulse(normDirectionTowardsCenter * 10.0f);
}

//-----------------------------------------------------------------------------------
void Wormhole::LinkWormholes(Wormhole* wormhole1, Wormhole* wormhole2)
{
    wormhole1->m_linkedWormhole = wormhole2;
    wormhole2->m_linkedWormhole = wormhole1;
}
