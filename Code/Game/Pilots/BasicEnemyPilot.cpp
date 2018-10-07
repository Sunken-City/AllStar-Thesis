#include "Game/Pilots/BasicEnemyPilot.hpp"
#include "../Entities/PlayerShip.hpp"
#include "../TheGame.hpp"

//-----------------------------------------------------------------------------------
BasicEnemyPilot::BasicEnemyPilot()
{
    m_inputMap.MapInputAxis("Up")->AddMapping(&m_movement.m_yAxis);
    m_inputMap.MapInputAxis("Right")->AddMapping(&m_movement.m_xAxis);
    m_inputMap.MapInputAxis("ShootUp")->AddMapping(&m_shooting.m_yAxis);
    m_inputMap.MapInputAxis("ShootRight")->AddMapping(&m_shooting.m_xAxis); 
    m_inputMap.MapInputValue("Shoot", ChordResolutionMode::RESOLVE_MAXS_ABSOLUTE);
    m_wanderDirection = MathUtils::GetRandomVectorInCircle(0.75f);
}

//-----------------------------------------------------------------------------------
BasicEnemyPilot::~BasicEnemyPilot()
{

}

//-----------------------------------------------------------------------------------
void BasicEnemyPilot::Update(float deltaSeconds, Ship* currentShip)
{
    m_currentShip = currentShip;
    if (!m_currentShip)
    {
        return;
    }

    m_timeSinceRetargetSeconds += deltaSeconds;
    m_timeSinceRewanderSeconds += deltaSeconds;

    if (m_timeSinceRetargetSeconds > TIME_IN_BETWEEN_TARGETING_SECONDS)
    {
        FindTarget();
        m_timeSinceRetargetSeconds = 0.0f;
    }
    if (m_timeSinceRewanderSeconds > TIME_IN_BETWEEN_WANDERING_SECONDS)
    {
        m_wanderDirection = MathUtils::GetRandomVectorInCircle(0.75f);
        m_timeSinceRewanderSeconds = MathUtils::GetRandomFloat(0.0f, 1.0f);
    }

    if (m_currentTarget)
    {
        Vector2 deltaPosition = m_currentTarget->GetPosition() - m_currentShip->GetPosition();
        Vector2 direction = deltaPosition.GetNorm();

        m_inputMap.FindInputAxis("Right")->SetValue(direction.x);
        m_inputMap.FindInputAxis("Up")->SetValue(direction.y);
        m_inputMap.FindInputAxis("ShootUp")->SetValue(direction.y);
        m_inputMap.FindInputAxis("ShootRight")->SetValue(direction.x);
        m_inputMap.FindInputValue("Shoot")->SetValue(true);
    }
    else
    {
        m_inputMap.FindInputAxis("Right")->SetValue(m_wanderDirection.x);
        m_inputMap.FindInputAxis("Up")->SetValue(m_wanderDirection.y);
        m_inputMap.FindInputAxis("ShootUp")->SetValue(0.0f);
        m_inputMap.FindInputAxis("ShootRight")->SetValue(0.0f);
        m_inputMap.FindInputValue("Shoot")->SetValue(false);
    }
}

//-----------------------------------------------------------------------------------
void BasicEnemyPilot::FindTarget()
{
    m_currentTarget = nullptr;
    float bestDistSquared = 9999999.0f;
    for (PlayerShip* player : TheGame::instance->m_players)
    {
        float distSquared = MathUtils::CalcDistSquaredBetweenPoints(player->GetPosition(), m_currentShip->GetPosition());
        float detectionRadius = DETECTION_RADIUS_SQUARED * (Ship::MAX_STEALTH_FACTOR - player->m_stealthFactor);
        if (distSquared < detectionRadius && distSquared < bestDistSquared)
        {
            bestDistSquared = distSquared;
            m_currentTarget = player;
        }
    }
}