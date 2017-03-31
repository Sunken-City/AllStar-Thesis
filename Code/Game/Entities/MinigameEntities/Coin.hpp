#pragma once
#include "Game/Entities/Entity.hpp"


class Coin : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Coin(const Vector2& position, int value = 0);
    virtual ~Coin();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity) override;
    inline virtual bool IsPickup() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr int GOLD_VALUE = 7;
    static constexpr int SILVER_VALUE = 3;
    static constexpr int BRONZE_VALUE = 1;

    float m_maxAge = 300.0f;
    Vector2 m_scale = Vector2(1.0f);
    int m_value = 0;
};
