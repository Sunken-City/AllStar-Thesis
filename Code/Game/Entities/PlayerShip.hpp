#pragma once
#include "Game/Entities/Ship.hpp"
#include "Game/Stats.hpp"
#include <stdint.h>
#include "Engine/Renderer/RGBA.hpp"

enum class PowerUpType;
class PlayerPilot;
class TextRenderable2D;

class PlayerShip : public Ship
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PlayerShip(PlayerPilot* playerPilot);
    virtual ~PlayerShip();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void Die();
    void Respawn();
    inline virtual bool IsPlayer() { return true; }
    void DropPowerups();
    void PickUpItem(Item* pickedUpItem);
    void DropRandomPowerup();
    RGBA GetPlayerColor();
    void HideUI();
    void ShowUI();
    void InitializeUI();
    
    //STAT FUNCTIONS/////////////////////////////////////////////////////////////////////
    inline virtual float GetTopSpeedStat() { return Ship::GetTopSpeedStat() + m_powerupStatModifiers.topSpeed; };
    inline virtual float GetAccelerationStat() { return Ship::GetAccelerationStat() + m_powerupStatModifiers.acceleration; };
    inline virtual float GetHandlingStat() { return Ship::GetHandlingStat() + m_powerupStatModifiers.handling; };
    inline virtual float GetBrakingStat() { return Ship::GetBrakingStat() + m_powerupStatModifiers.braking; };
    inline virtual float GetDamageStat() { return Ship::GetDamageStat() + m_powerupStatModifiers.damage; };
    inline virtual float GetShieldDisruptionStat() { return Ship::GetShieldDisruptionStat() + m_powerupStatModifiers.shieldDisruption; };
    inline virtual float GetShotHomingStat() { return Ship::GetShotHomingStat() + m_powerupStatModifiers.shotHoming; };
    inline virtual float GetRateOfFireStat() { return Ship::GetRateOfFireStat() + m_powerupStatModifiers.rateOfFire; };
    inline virtual float GetHpStat() { return Ship::GetHpStat() + m_powerupStatModifiers.hp; };
    inline virtual float GetShieldCapacityStat() { return Ship::GetShieldCapacityStat() + m_powerupStatModifiers.shieldCapacity; };
    inline virtual float GetShieldRegenStat() { return Ship::GetShieldRegenStat() + m_powerupStatModifiers.shieldRegen; };
    inline virtual float GetShotDeflectionStat() { return Ship::GetShotDeflectionStat() + m_powerupStatModifiers.shotDeflection; };
    void Eject(Item* item);
    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Stats m_powerupStatModifiers;
    Sprite* m_speedometer = nullptr;
    TextRenderable2D* m_healthText = nullptr;
    TextRenderable2D* m_shieldText = nullptr;
    TextRenderable2D* m_speedText = nullptr;
};