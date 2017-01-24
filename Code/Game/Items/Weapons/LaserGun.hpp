#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class LaserGun : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    LaserGun();
    virtual ~LaserGun();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    unsigned int m_numMisslesPerShot = 3;
    float m_shotHomingBonus = 50.0f;
    float m_rateOfFireBonus = -3.0f;
};
