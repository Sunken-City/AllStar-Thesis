#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class MissileLauncher : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    MissileLauncher();
    virtual ~MissileLauncher();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    unsigned int m_numMisslesPerShot = 3;
    float m_shotHomingBonus = 50.0f;
    float m_rateOfFireBonus = -10.0f;
};