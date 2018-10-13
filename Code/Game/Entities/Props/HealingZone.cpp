#include "Game/Entities/Props/HealingZone.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "../Ship.hpp"
#include <algorithm>
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "../TextSplash.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

const float HealingZone::MAX_ANGULAR_VELOCITY = 1360.0f;
const float HealingZone::MAX_POINTS_OF_HEALING = 500.0f;
const float HealingZone::HEAL_AMOUNT_PER_COLLISION = 0.50f;

//-----------------------------------------------------------------------------------
HealingZone::HealingZone(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MAX_ANGULAR_VELOCITY)
    , m_remainingHealing(MAX_POINTS_OF_HEALING)
{
    m_sprite = new Sprite("HealingZone", TheGame::FOREGROUND_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
}

//-----------------------------------------------------------------------------------
HealingZone::~HealingZone()
{

}

//-----------------------------------------------------------------------------------
void HealingZone::Update(float deltaSeconds)
{
    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);
    float colorInterpValue = Lerp(m_remainingHealing / MAX_POINTS_OF_HEALING, 0.1f, 1.0f);
    m_angularVelocity = Lerp(m_angularVelocity / MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY, 0.0f);
    m_sprite->m_tintColor = RGBA(Vector4(Vector3(colorInterpValue), 1.0f));
    if (m_remainingHealing < 0.0f)
    {
        Die();
    }
}

//-----------------------------------------------------------------------------------
void HealingZone::ResolveCollision(Entity* otherEntity)
{
    Ship* otherShip = dynamic_cast<Ship*>(otherEntity);
    if (otherShip && !otherShip->HasFullHealth())
    {
        otherShip->Heal(HEAL_AMOUNT_PER_COLLISION);
        m_remainingHealing -= HEAL_AMOUNT_PER_COLLISION;

        float randomPercentage = MathUtils::GetRandomFloatFromZeroTo(0.2f);
        float randomDegrees = MathUtils::GetRandomFloat(-70.0f, 70.0f);
        Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;
        ParticleSystem::PlayOneShotParticleEffect("Healing", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(otherShip->m_transform.GetWorldPosition()));
    }
}
