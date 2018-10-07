#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Pilot.hpp"
class Ship;

class BasicEnemyPilot : public Pilot
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BasicEnemyPilot();
    virtual ~BasicEnemyPilot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds, Ship* currentShip);
    void FindTarget();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_timeSinceRetargetSeconds = 0.0f;
    float m_timeSinceRewanderSeconds = 0.0f;
    float m_angularVelocity;
    Vector2 m_wanderDirection = Vector2::ZERO;
    Ship* m_currentTarget = nullptr;
    Ship* m_currentShip = nullptr;
    InputVector2 m_movement;
    InputVector2 m_shooting;
    InputValue m_shouldShoot = false;

    //STATIC VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr float TIME_IN_BETWEEN_TARGETING_SECONDS = 0.5f;
    static constexpr float TIME_IN_BETWEEN_WANDERING_SECONDS = 3.0f;
    static constexpr float DETECTION_RADIUS_SQUARED = 60.0f;
};