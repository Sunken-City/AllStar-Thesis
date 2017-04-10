#include "Game/Entities/Projectiles/Explosion.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "../PlayerShip.hpp"
#include "Missile.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"

const float Explosion::KNOCKBACK_MAGNITUDE = 10.0f;

//-----------------------------------------------------------------------------------
Explosion::Explosion(Entity* owner, Entity* creator, const Vector2& spawnPosition, float damage /*= 1.0f*/, float disruption /*= 0.0f*/)
    : Projectile(owner, 0.0f, damage, disruption, 0.0f)
{
    static const SoundID explosionSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/missileExplosion.wav");
    m_sprite = new Sprite("YellowCircle", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetPosition(spawnPosition);
    m_transform.SetScale(Vector2(0.0f));
    CalculateCollisionRadius();

    m_velocity = Vector2::ZERO;
    m_collisionDamageAmount = damage;
    m_isImmobile = true;
    GameMode::GetCurrent()->PlaySoundAt(explosionSound, spawnPosition, TheGame::BULLET_VOLUME, MathUtils::GetRandomFloat(0.9f, 1.1f));

    Missile* missilePtr = dynamic_cast<Missile*>(creator);
    if (missilePtr)
    {
        m_particleTrail = missilePtr->m_missileTrail;
        m_particleTrail->m_emitters[0]->m_transform.SetParent(&m_transform);
    }
}

//-----------------------------------------------------------------------------------
Explosion::~Explosion()
{
    if (m_particleTrail)
    {
        delete m_particleTrail;
        m_particleTrail = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void Explosion::Update(float deltaSeconds)
{
    Projectile::Update(deltaSeconds);
    float theta = Lerp<float>(m_age / m_lifeSpan, 0.0f, 180.0f);
    float t = MathUtils::SinDegrees(theta);
    m_transform.SetScale(Vector2(MAX_SCALE * t));
    CalculateCollisionRadius();
}

//-----------------------------------------------------------------------------------
float Explosion::GetKnockbackMagnitude()
{
    return KNOCKBACK_MAGNITUDE;
}

//-----------------------------------------------------------------------------------
void Explosion::ResolveCollision(Entity* otherEntity)
{
    if (otherEntity != m_owner && otherEntity->m_collidesWithBullets && !otherEntity->m_isDead)
    {
        Entity::ResolveCollision(otherEntity);
        Vector2 dispFromThisToOther = otherEntity->m_transform.GetWorldPosition() - m_transform.GetWorldPosition();
        otherEntity->ApplyImpulse(dispFromThisToOther.GetNorm() * GetKnockbackMagnitude());
    }
}

//-----------------------------------------------------------------------------------
bool Explosion::FlushParticleTrailIfExists()
{
    if (m_particleTrail)
    {
        m_particleTrail->Flush();
        return true;
    }
    return false;
}

