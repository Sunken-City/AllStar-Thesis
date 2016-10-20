#pragma once
#include "Game/Entities/Prop.hpp"

class ItemBox : public Prop
{
public:
    ItemBox(const Vector2& position);
    virtual ~ItemBox();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};