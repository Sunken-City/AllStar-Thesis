#pragma once
#include "Game/Entities/Entity.hpp"

class Sprite;
class Pilot;
class Vector2;
class ParticleSystem;

class Ship : public Entity
{
public:
    Ship(Pilot* pilot = nullptr);
    virtual ~Ship();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void LockMovement() { m_lockMovement = true; m_velocity = Vector2::ZERO; };
    virtual void UnlockMovement() { m_lockMovement = false; };
    virtual void ToggleMovement() { m_lockMovement = !m_lockMovement; };
    virtual float TakeDamage(float damage, float disruption = 1.0f) override;
    virtual void Heal(float healValue = 99999999.0f) override;
    virtual void Die();
    virtual Vector2 GetMuzzlePosition();
    virtual const SpriteResource* GetCollisionSpriteResource() override;
    void UpdateMotion(float deltaSeconds);
    void UpdateShooting();
    void RegenerateShield(float deltaSeconds);
    void ApplyShotDeflection();
    void FlickerShield(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Pilot* m_pilot;
    ParticleSystem* m_shipTrail = nullptr;
    ParticleSystem* m_smokeDamage = nullptr;
    const SpriteResource* m_shieldCollisionSpriteResource = nullptr;
    float m_muzzleOffsetMagnitude = 0.25f;
    float m_secondsSinceLastFiredWeapon;
    float m_hitSoundMaxVolume = 0.7f;
    bool m_lockMovement = false;
};