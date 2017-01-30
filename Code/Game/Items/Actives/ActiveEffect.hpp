#pragma once
#include "Game/Items/Item.hpp"
#include "Engine/Core/Events/NamedProperties.hpp"

class ActiveEffect : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    ActiveEffect();
    virtual ~ActiveEffect();
    virtual void Update(float deltaSeconds) = 0;
    virtual void Activate(NamedProperties& parameters) = 0;
    virtual void Deactivate(NamedProperties& parameters) = 0;

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual void Cooldown(float deltaSeconds);
    virtual inline bool IsActive() { return m_isActive; };
    virtual inline bool CanActivate() { return (m_energy >= m_costToActivate) && (!m_isActive); };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    double m_lastActivatedMiliseconds = -10.0f;
    float m_energy = 1.0f;
    float m_energyRestorationPerSecond = 0.0f;
    float m_costToActivate = 0.0f;
    bool m_isActive = false;
};