#include "Game/Entities/Ship.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship()
    : Entity()
    , m_speed(0.0f)
    , m_power(0.0f)
    , m_rateOfFire(0.0f)
    , m_timeSinceLastShot(0.0f)
{

}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{
}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_timeSinceLastShot += deltaSeconds;
}

//-----------------------------------------------------------------------------------
void Ship::Render() const
{

}

//-----------------------------------------------------------------------------------
void Ship::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
}

