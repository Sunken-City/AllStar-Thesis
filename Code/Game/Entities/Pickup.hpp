#pragma once
#include "Game/Items/Item.hpp"
#include "Game/Entities/Entity.hpp"

class Pickup : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Pickup(Item* item, const Vector2& position);
    virtual ~Pickup();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Item* m_item;
    float m_maxAge = 30.0f;
};