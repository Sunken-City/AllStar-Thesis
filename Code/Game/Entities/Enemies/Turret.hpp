#pragma once
#include "Game/Entities/Ship.hpp"

class Turret : public Ship
{
public:
    Turret(const Vector2& position);
    virtual ~Turret();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void Die();
};