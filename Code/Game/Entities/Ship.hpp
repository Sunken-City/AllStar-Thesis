#pragma once
#include "Game/Entities/Entity.hpp"

class Sprite;
class Pilot;
class Vector2;

class Ship : public Entity
{
public:
    Ship(Pilot* pilot = nullptr);
    virtual ~Ship();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void LockMovement() { m_lockMovement = true; };
    virtual void UnlockMovement() { m_lockMovement = false; };
    virtual void ToggleMovement() { m_lockMovement = !m_lockMovement; };
    virtual void TakeDamage(float damage, float disruption, float penetration);
    virtual void Die();
    void UpdateMotion(float deltaSeconds);
    void UpdateShooting();
    void RegenerateShield(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Pilot* m_pilot;
    float m_secondsSinceLastFiredWeapon;
    float m_hitSoundMaxVolume = 0.7f;
    bool m_lockMovement = false;
};