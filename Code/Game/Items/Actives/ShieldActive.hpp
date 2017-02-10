#pragma once
#include "Game/Items/Actives/ActiveEffect.hpp"

class Ship;

class ShieldActive : public ActiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    ShieldActive();
    virtual ~ShieldActive();
    virtual void Update(float deltaSeconds);
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const double SECONDS_DURATION;
    static const double MILISECONDS_DURATION;
};


