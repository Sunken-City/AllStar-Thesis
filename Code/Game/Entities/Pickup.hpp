#pragma once
#include "Game/Items/Item.hpp"
#include "Game/Entities/Entity.hpp"

class TextRenderable2D;

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
    inline virtual bool IsPickup() { return true; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Item* m_item;
    TextRenderable2D* m_descriptionTextRenderable;
    TextRenderable2D* m_equipTextRenderable;
    float m_maxAge = 30.0f;

};