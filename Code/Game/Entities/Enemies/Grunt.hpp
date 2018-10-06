#pragma once
#include "Game/Entities/Ship.hpp"

class Grunt : public Ship
{
public:
    Grunt(const Vector2& position);
    virtual ~Grunt();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void Die();
    void FindTarget();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;
    float m_timeSinceRetargetSeconds = 0.0f;
    Ship* m_currentTarget = nullptr;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static constexpr float DETECTION_RADIUS_SQUARED = 49.0f;
    static constexpr float TIME_IN_BETWEEN_TARGETING_SECONDS = 0.5f;
};