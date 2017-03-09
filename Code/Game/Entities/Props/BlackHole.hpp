#pragma once
#include "Game/Entities/Entity.hpp"

class BlackHole : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BlackHole(const Vector2& position);
    virtual ~BlackHole();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity) override;
    inline virtual bool IsProp() override { return true; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Sprite* m_overlaySprite = nullptr;
    float m_angularVelocity;
    bool m_growsOverTime = false;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const float PERCENTAGE_RADIUS_INNER_RADIUS;
    static const float SCALE_GROWTH_RATE;
};

