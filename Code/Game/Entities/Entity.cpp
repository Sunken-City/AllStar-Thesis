#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"

//-----------------------------------------------------------------------------------
Entity::Entity()
    : m_sprite(nullptr)
    , m_defence(0.0f)
    , m_hp(1.0f)
    , m_maxHp(1.0f)
    , m_collisionRadius(1.0f)
    , m_age(0.0f)
    , m_isDead(false)
{

}

//-----------------------------------------------------------------------------------
Entity::~Entity()
{
    if (m_sprite)
    {
        delete m_sprite;
    }
}

//-----------------------------------------------------------------------------------
void Entity::Update(float deltaSeconds)
{
    m_age += deltaSeconds;
}

//-----------------------------------------------------------------------------------
void Entity::Render() const
{

}

//-----------------------------------------------------------------------------------
bool Entity::IsCollidingWith(Entity* otherEntity)
{
    return this->m_sprite->GetBounds().IsIntersecting(otherEntity->m_sprite->GetBounds());
}

//-----------------------------------------------------------------------------------
void Entity::ResolveCollision(Entity* otherEntity)
{
    Vector2& myPosition = this->m_sprite->m_position;
    Vector2 otherPosition = otherEntity->m_sprite->m_position;
    Vector2 difference = myPosition - otherPosition;
    float distanceBetweenPoints = MathUtils::CalcDistanceBetweenPoints(otherPosition, myPosition);
    float firstPushDist = (this->m_collisionRadius - distanceBetweenPoints) / 8.f;
    //float secondPushDist = (otherEntity->m_collisionRadius - distanceBetweenPoints) / 8.f;
    difference *= -firstPushDist;
    myPosition -= difference;
}

//-----------------------------------------------------------------------------------
void Entity::TakeDamage(float damage)
{
    m_hp -= damage;
    if (m_hp < 0.0f)
    {
        m_isDead = true;
    }
}

