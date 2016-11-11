#pragma once
#include "Game/Entities/Entity.hpp"

class ItemCrate : public Entity
{
public:
    ItemCrate(const Vector2& position);
    virtual ~ItemCrate();

    virtual void Update(float deltaSeconds);
    void GenerateItems();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const int MAX_NUM_PICKUPS_PER_BOX = 5;
};