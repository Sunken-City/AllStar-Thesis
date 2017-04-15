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
    virtual void ResolveCollision(Entity* otherEntity) override;
    inline virtual bool IsPickup() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Item* m_item;
    TextRenderable2D* m_typeTextRenderable;
    TextRenderable2D* m_descriptionTextRenderable;
    TextRenderable2D* m_equipTextRenderable;
    Vector2 m_baseScale = Vector2::ONE;
    float m_maxAge = 30.0f;

};