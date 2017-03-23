#pragma once
#include "Game/Entities/Entity.hpp"

class ParticleSystem;

class ShipDebris : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    ShipDebris(const Transform2D& transform, const SpriteResource* resource, const Vector2& velocity);
    virtual ~ShipDebris();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    inline virtual bool IsProp() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    ParticleSystem* m_smokeDamage = nullptr;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
};
