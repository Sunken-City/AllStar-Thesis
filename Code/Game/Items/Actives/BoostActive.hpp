#pragma once
#include "Game/Items/Actives/ActiveEffect.hpp"

class Ship;
class PlayerShip;

class BoostActive : public ActiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BoostActive();
    virtual ~BoostActive();
    virtual void Update(float deltaSeconds);
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const double SECONDS_DURATION;
    static const double MILISECONDS_DURATION;
    static constexpr float BOOST_DAMAGE_PER_FRAME = 20.0f;

    PlayerShip* m_owner = nullptr;
};


