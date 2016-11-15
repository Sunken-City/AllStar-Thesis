#include "Game/Entities/Projectile.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"

//-----------------------------------------------------------------------------------
Projectile::Projectile(Entity* owner) 
    : Entity()
    , m_speed(10.0f)
    , m_power(1.0f)
    , m_lifeSpan(2.0f)
{
    m_owner = owner;
    m_collidesWithBullets = false;
    m_sprite = new Sprite("Laser", TheGame::PLAYER_BULLET_LAYER);
    m_sprite->m_scale = Vector2(1.0f, 1.0f);
    CalculateCollisionRadius();

    SetPosition(owner->GetPosition());
    m_sprite->m_rotationDegrees = m_owner->m_sprite->m_rotationDegrees;

    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_rotationDegrees, Vector2::ZERO_DEGREES_UP);
    Vector2 muzzleVelocity = direction * m_speed;
    m_velocity = muzzleVelocity + m_owner->m_velocity;
}

//-----------------------------------------------------------------------------------
Projectile::~Projectile()
{
}

//-----------------------------------------------------------------------------------
void Projectile::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    if (m_age < m_lifeSpan)
    {
        Vector2 position = GetPosition();
        SetPosition(position + m_velocity * deltaSeconds);
    }
    else
    {
        m_isDead = true;
    }
}

//-----------------------------------------------------------------------------------
void Projectile::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
    if (otherEntity != m_owner && otherEntity->m_collidesWithBullets && !otherEntity->m_isDead)
    {
        otherEntity->TakeDamage(m_power);
        this->m_isDead = true;
    }
}

