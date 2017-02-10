#include "Game/Pilots/Pilot.hpp"

//-----------------------------------------------------------------------------------
Pilot::Pilot()
{

}

//-----------------------------------------------------------------------------------
Pilot::~Pilot()
{
    m_inputMap.Clear();
}

//-----------------------------------------------------------------------------------
void Pilot::RecoilScreenshake(float /*magnitude*/, const Vector2& /*direction*/)
{

}

//-----------------------------------------------------------------------------------
void Pilot::LightRumble(float /*amount*/, float /*secondsDuration*/ /*= 0.25f*/)
{

}

//-----------------------------------------------------------------------------------
void Pilot::HeavyRumble(float /*amount*/, float /*secondsDuration*/ /*= 0.25f*/)
{

}

