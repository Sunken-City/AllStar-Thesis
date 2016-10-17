#pragma once
#include "Game/Entities/Ship.hpp"
#include <stdint.h>

class Player : public Ship
{
public:
    Player();
    ~Player();
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    uint8_t m_netOwnerIndex;
};