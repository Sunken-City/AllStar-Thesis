#pragma once
#include "Weapons\Weapon.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Weapons\MissileLauncher.hpp"
#include "Weapons\LaserGun.hpp"
#include "Chassis\Chassis.hpp"
#include "Chassis\DefaultChassis.hpp"
#include "Chassis\SpeedChassis.hpp"
#include "Actives\ActiveEffect.hpp"
#include "Actives\WarpActive.hpp"
#include "Passives\PassiveEffect.hpp"
#include "Passives\CloakPassive.hpp"
#include "Passives\StealthTrailPassive.hpp"

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
        return new LaserGun();
    }
}

//-----------------------------------------------------------------------------------
Chassis* GetRandomChassis()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(2);
    if (randomNumber == 0)
    {
        return new DefaultChassis();
    }
    else
    {
        return new SpeedChassis();
    }
}

//-----------------------------------------------------------------------------------
ActiveEffect* GetRandomActive()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(2);
    if (randomNumber == 0)
    {
        //return new DefaultChassis();
    }
    else
    {
        //return new SpeedChassis();
    }
    return new WarpActive();
}

//-----------------------------------------------------------------------------------
PassiveEffect* GetRandomPassive()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(2);
    if (randomNumber == 0)
    {
        return new CloakPassive();
    }
    else
    {
        return new StealthTrailPassive();
    }
}