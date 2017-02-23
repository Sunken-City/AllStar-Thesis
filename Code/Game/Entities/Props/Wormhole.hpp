#pragma once
#include "Game/Entities/Entity.hpp"

class Wormhole : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Wormhole(const Vector2& position);
    virtual ~Wormhole();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity) override;
    static void LinkWormholes(Wormhole* wormhole1, Wormhole* wormhole2);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;
    Wormhole* m_linkedWormhole;
    Sprite* m_overlaySprite;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const float PERCENTAGE_RADIUS_INNER_RADIUS;
};
