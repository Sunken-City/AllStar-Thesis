#pragma once
#include "Weapons\Weapon.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Weapons\MissileLauncher.hpp"
#include "Weapons\LaserGun.hpp"
#include "Chassis\Chassis.hpp"
#include "Chassis\BlackHoleChassis.hpp"
#include "Chassis\SpeedChassis.hpp"
#include "Actives\ActiveEffect.hpp"
#include "Actives\TeleportActive.hpp"
#include "Actives\QuickshotActive.hpp"
#include "Passives\PassiveEffect.hpp"
#include "Passives\CloakPassive.hpp"
#include "Passives\StealthTrailPassive.hpp"
#include "Actives\WarpActive.hpp"
#include "Passives\SpecialTrailPassive.hpp"
#include "Actives\BoostActive.hpp"
#include "Actives\ShieldActive.hpp"
#include "Chassis\TankChassis.hpp"
#include "Chassis\GlassCannonChassis.hpp"
#include "Chassis\PowerChassis.hpp"

//-----------------------------------------------------------------------------------
Weapon* GetRandomWeapon()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(2);
    if (randomNumber == 0)
    {
        return new MissileLauncher();
    }
    else
    {
        return new MissileLauncher();
    }
}

//-----------------------------------------------------------------------------------
Chassis* GetRandomChassis()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(2);
    if (randomNumber == 0)
    {
        return new SpeedChassis();
    }
    else if (randomNumber == 1)
    {
        return new BlackHoleChassis();
    }
    else if (randomNumber == 2)
    {
        return new GlassCannonChassis();
    }
    else if (randomNumber == 3)
    {
        return new PowerChassis();
    }
    else
    {
        return new TankChassis();
    }
}

//-----------------------------------------------------------------------------------
ActiveEffect* GetRandomActive()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(5);
    if (randomNumber == 0)
    {
        return new TeleportActive();
    }
    else if (randomNumber == 1)
    {
        return new WarpActive();
    }
    else if (randomNumber == 2)
    {
        return new QuickshotActive();
    }
    else if (randomNumber == 3)
    {
        return new BoostActive();
    }
    else
    {
        return new ShieldActive();
    }
}

//-----------------------------------------------------------------------------------
PassiveEffect* GetRandomPassive()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(3);
    if (randomNumber == 0)
    {
        return new CloakPassive();
    }
    else if (randomNumber == 1)
    {
        return new SpecialTrailPassive();
    }
    else
    {
        return new StealthTrailPassive();
    }
}