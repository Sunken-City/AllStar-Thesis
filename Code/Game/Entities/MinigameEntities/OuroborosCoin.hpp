#pragma once
#include "Coin.hpp"

class PlayerShip;

class OuroborosCoin : public Coin
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    OuroborosCoin(PlayerShip* owner, const Vector2& position, int value = OUROBOROS_VALUE);
    virtual ~OuroborosCoin();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity) override;
    inline virtual bool IsPickup() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr int OUROBOROS_VALUE = 2;
    static constexpr int WINNER_OUROBOROS_VALUE = 5;
    PlayerShip* m_owner;
};
