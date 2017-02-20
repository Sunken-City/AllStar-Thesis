#include "Game/Entities/Props/BlackHole.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Time/Time.hpp"

const float BlackHole::MAX_ANGULAR_VELOCITY = 20.0f;
const float BlackHole::PERCENTAGE_RADIUS_INNER_RADIUS = 0.1f;

//-----------------------------------------------------------------------------------
BlackHole::BlackHole(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Wormhole", TheGame::BACKGROUND_GEOMETRY_LAYER);
    m_sprite->m_tintColor = RGBA::CHOCOLATE;
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
}

//-----------------------------------------------------------------------------------
BlackHole::~BlackHole()
{

}

//-----------------------------------------------------------------------------------
void BlackHole::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    float newRotationDegrees = m_sprite->m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_sprite->m_transform.SetRotationDegrees(newRotationDegrees);
}

//-----------------------------------------------------------------------------------
void BlackHole::ResolveCollision(Entity* otherEntity)
{
    const float COLLISION_RADIUS_SQUARED = m_collisionRadius * m_collisionRadius;
    const float INNER_RADIUS_SQUARED = COLLISION_RADIUS_SQUARED * PERCENTAGE_RADIUS_INNER_RADIUS;
    Vector2 dispFromOtherToCenter = m_transform.GetWorldPosition() - otherEntity->m_transform.GetWorldPosition();
    Vector2 normDirectionTowardsCenter = dispFromOtherToCenter.GetNorm();

    if (dispFromOtherToCenter.CalculateMagnitudeSquared() < INNER_RADIUS_SQUARED)
    {
        if (otherEntity->IsProjectile())
        {
            otherEntity->Die();
        }
        else
        {
            otherEntity->TakeDamage(5.0f);
        }
    }
    else
    {
        otherEntity->ApplyImpulse(normDirectionTowardsCenter * 10.0f);
    }

}