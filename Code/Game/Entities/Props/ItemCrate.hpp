#pragma once
#include "Game/Entities/Entity.hpp"

class ItemCrate : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    ItemCrate(const Vector2& position);
    virtual ~ItemCrate();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds) override;
    virtual void Die() override;
    void GenerateItems();
    void DecorateCrate();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Sprite* m_itemHintSprite = nullptr;
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const int MAX_NUM_PICKUPS_PER_BOX = 5;
};