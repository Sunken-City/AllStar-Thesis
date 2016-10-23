#pragma once
#include "Game/Entities/Prop.hpp"

class ItemCrate : public Prop
{
public:
    ItemCrate(const Vector2& position);
    virtual ~ItemCrate();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    void GenerateItems();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const int MAX_NUM_PICKUPS_PER_BOX = 5;
};