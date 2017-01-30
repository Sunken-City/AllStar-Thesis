#pragma once
#include "Game/Items/Item.hpp"
#include "Engine/Core/Events/NamedProperties.hpp"

class Ship;

class PassiveEffect : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PassiveEffect();
    virtual ~PassiveEffect();
    virtual void Update(float deltaSeconds) = 0;
    virtual void Activate(NamedProperties& parameters) = 0;
    virtual void Deactivate(NamedProperties& parameters) = 0;

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Ship* m_owner = nullptr;
};