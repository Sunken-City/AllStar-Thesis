#include "Game/Entities/Props/Wormhole.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Time/Time.hpp"

const float Wormhole::MAX_ANGULAR_VELOCITY = 20.0f;
const float Wormhole::PERCENTAGE_RADIUS_INNER_RADIUS = 0.1f;

//-----------------------------------------------------------------------------------
Wormhole::Wormhole(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Wormhole", TheGame::BACKGROUND_GEOMETRY_LAYER);
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
    const float GRACE_PERIOD_TELEPORT_SECONDS = 0.5f;
    const float COLLISION_RADIUS_SQUARED = m_collisionRadius * m_collisionRadius;
    const float INNER_RADIUS_SQUARED = COLLISION_RADIUS_SQUARED * PERCENTAGE_RADIUS_INNER_RADIUS;
    Vector2 dispFromOtherToCenter = m_transform.GetWorldPosition() - otherEntity->m_transform.GetWorldPosition();
    Vector2 normDirectionTowardsCenter = dispFromOtherToCenter.GetNorm();

    if ((dispFromOtherToCenter.CalculateMagnitudeSquared() < INNER_RADIUS_SQUARED) && (GetCurrentTimeSeconds() - otherEntity->m_timeLastWarped > GRACE_PERIOD_TELEPORT_SECONDS))
    {
        ASSERT_OR_DIE(m_linkedWormhole, "Wormhole wasn't linked to another!");
        otherEntity->m_transform.SetPosition(m_linkedWormhole->m_transform.GetWorldPosition() + normDirectionTowardsCenter * 1.0f);
        otherEntity->ApplyImpulse(normDirectionTowardsCenter * 100.0f);
        otherEntity->m_timeLastWarped = GetCurrentTimeSeconds();
    }
    else
    {
        otherEntity->ApplyImpulse(normDirectionTowardsCenter * 10.0f);
    }

}

//-----------------------------------------------------------------------------------
void Wormhole::Teleport(Entity* otherEntity)
{
}

//-----------------------------------------------------------------------------------
void Wormhole::LinkWormholes(Wormhole* wormhole1, Wormhole* wormhole2)
{
    wormhole1->m_linkedWormhole = wormhole2;
    wormhole2->m_linkedWormhole = wormhole1;
}
