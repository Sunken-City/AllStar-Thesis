#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
Encounter::Encounter(const Vector2& center, float radius)
    : m_center(center)
    , m_radius(radius)
{

}

//-----------------------------------------------------------------------------------
Vector2 Encounter::CalculateSpawnPosition(const Vector2& relative01Position)
{
    return m_center + (relative01Position * m_radius);
}
