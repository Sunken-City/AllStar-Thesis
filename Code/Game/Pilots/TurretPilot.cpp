#include "Game/Pilots/TurretPilot.hpp"
#include "../Entities/PlayerShip.hpp"
#include "../TheGame.hpp"

//-----------------------------------------------------------------------------------
TurretPilot::TurretPilot()
{
}

//-----------------------------------------------------------------------------------
TurretPilot::~TurretPilot()
{

}

//-----------------------------------------------------------------------------------
void TurretPilot::Update(float deltaSeconds, Ship* currentShip)
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

        m_inputMap.FindInputAxis("ShootUp")->SetValue(direction.y);
        m_inputMap.FindInputAxis("ShootRight")->SetValue(direction.x);
        m_inputMap.FindInputValue("Shoot")->SetValue(true);
    }
    else
    {
        m_inputMap.FindInputAxis("ShootRight")->SetValue(m_wanderDirection.x);
        m_inputMap.FindInputAxis("ShootUp")->SetValue(m_wanderDirection.y);
        m_inputMap.FindInputValue("Shoot")->SetValue(false);
    }
}