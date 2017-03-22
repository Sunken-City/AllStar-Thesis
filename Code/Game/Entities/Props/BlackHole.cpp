#include "Game/Entities/Props/BlackHole.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Math/Vector3.hpp"

const float BlackHole::MAX_ANGULAR_VELOCITY = 20.0f;
const float BlackHole::PERCENTAGE_RADIUS_INNER_RADIUS = 0.1f;
const float BlackHole::SCALE_GROWTH_RATE = 0.1f;


//-----------------------------------------------------------------------------------
BlackHole::BlackHole(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Wormhole", TheGame::BACKGROUND_GEOMETRY_LAYER);
    m_overlaySprite = new Sprite("Wormhole", TheGame::FOREGROUND_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    m_overlaySprite->m_transform.SetParent(&m_transform);
    m_sprite->m_tintColor = RGBA::CHOCOLATE;
    m_overlaySprite->m_tintColor = RGBA::CHOCOLATE;
    m_overlaySprite->m_tintColor.SetAlphaFloat(0.5f);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_overlaySprite->m_transform.SetRotationDegrees(180.0f);
    m_overlaySprite->m_transform.SetScale(Vector2(1.1f, 1.1f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
    m_vortexID = GameMode::GetCurrent()->GetNextVortexID();
}

//-----------------------------------------------------------------------------------
BlackHole::~BlackHole()
{
    delete m_overlaySprite;
}

//-----------------------------------------------------------------------------------
void BlackHole::Update(float deltaSeconds)
{
    if (m_growsOverTime)
    {
        m_transform.SetScale(m_transform.GetWorldScale() + Vector2(SCALE_GROWTH_RATE * deltaSeconds));
        CalculateCollisionRadius();
    }

    Entity::Update(deltaSeconds);
    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);
    m_overlaySprite->m_transform.SetRotationDegrees(-4.5313f * newRotationDegrees);
}

//-----------------------------------------------------------------------------------
void BlackHole::ResolveCollision(Entity* otherEntity)
{
    const float COLLISION_RADIUS_SQUARED = m_collisionRadius * m_collisionRadius;
    const float INNER_RADIUS_SQUARED = COLLISION_RADIUS_SQUARED * PERCENTAGE_RADIUS_INNER_RADIUS;
    Vector2 dispFromOtherToCenter = m_transform.GetWorldPosition() - otherEntity->m_transform.GetWorldPosition();
    Vector2 normDirectionTowardsCenter = dispFromOtherToCenter.GetNorm();

    otherEntity->SetVortexShaderPosition(m_transform.GetWorldPosition(), m_vortexID, m_collisionRadius);

    otherEntity->m_sprite->m_material->SetVec3Uniform("gWarpPosition", Vector3(m_transform.GetWorldPosition(), 0.0f));
    otherEntity->m_shieldSprite->m_material->SetVec3Uniform("gWarpPosition", Vector3(m_transform.GetWorldPosition(), 0.0f));

    if (dispFromOtherToCenter.CalculateMagnitudeSquared() < INNER_RADIUS_SQUARED)
    {
        if (otherEntity->IsProjectile())
        {
            otherEntity->Die();
        }
        else if (otherEntity->IsAlive())
        {
            otherEntity->TakeDamage(5.0f);
        }
    }
    else
    {
        otherEntity->ApplyImpulse(normDirectionTowardsCenter * 10.0f);
    }

}