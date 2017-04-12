#include "Game/Entities/Props/ShipDebris.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"

//-----------------------------------------------------------------------------------
ShipDebris::ShipDebris(const Transform2D& transform, const SpriteResource* resource, const Vector2& velocity) 
    : Entity()
{
    m_transform = transform;
    m_velocity = velocity;
    m_smokeDamage = new ParticleSystem("Smoking", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform);

    m_sprite = new Sprite("DefaultChassis", TheGame::BACKGROUND_GEOMETRY_LAYER_ABOVE);
    m_sprite->m_tintColor = RGBA::GRAY;
    m_sprite->m_spriteResource = resource;
    m_sprite->m_transform.SetParent(&m_transform);
    CalculateCollisionRadius();

    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleBrown");

    m_baseStats.hp = 10.0f;
    Heal();
}

//-----------------------------------------------------------------------------------
ShipDebris::~ShipDebris()
{
    m_transform.DropChildrenInPlace();
    ParticleSystem::DestroyImmediately(m_smokeDamage);
}

//-----------------------------------------------------------------------------------
void ShipDebris::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    Vector2 pos = m_transform.GetWorldPosition();
    pos += m_velocity * deltaSeconds;
    m_velocity *= 0.95f;
    SetPosition(pos);
}
