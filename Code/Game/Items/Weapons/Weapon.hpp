#pragma once
#include "Game/Items/Item.hpp"

class Ship;

class Weapon : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Weapon();
    virtual ~Weapon();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() = 0;
    virtual bool AttemptFire(Ship* shooter) = 0;
    virtual inline float GetKnockbackMagnitude() { return m_knockbackPerBullet * m_numProjectilesPerShot; };
    inline virtual const char* GetTypeText() { return "WEAPON"; };
    inline virtual RGBA GetTypeColor() { return RGBA::RED; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    unsigned int m_numProjectilesPerShot = 1;
    float m_spreadDegrees = 0.0f;
    float m_knockbackPerBullet = 10.0f;
};