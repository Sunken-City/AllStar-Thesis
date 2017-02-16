#include "Game/Entities/Props/Asteroid.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

const float Asteroid::MAX_ANGULAR_VELOCITY = 15.0f;
const float Asteroid::MIN_ASTEROID_SCALE = 0.5f;
const float Asteroid::MAX_ASTEROID_SCALE = 1.5f;

//-----------------------------------------------------------------------------------
Asteroid::Asteroid(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Asteroid", TheGame::GEOMETRY_LAYER);
    m_transform.SetScale(Vector2(MathUtils::GetRandomFloat(MIN_ASTEROID_SCALE, MAX_ASTEROID_SCALE)));
    m_sprite->m_transform.SetScale(m_transform.GetWorldScale());
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleBrown");
}

//-----------------------------------------------------------------------------------
Asteroid::~Asteroid()
{

}

//-----------------------------------------------------------------------------------
void Asteroid::Update(float deltaSeconds)
{
    float newRotationDegrees = m_sprite->m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_sprite->m_transform.SetRotationDegrees(newRotationDegrees);
    Vector2 direction = Vector2::DegreesToDirection(-newRotationDegrees, Vector2::ZERO_DEGREES_UP);
}
