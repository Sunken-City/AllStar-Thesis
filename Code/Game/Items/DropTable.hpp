#pragma once
#include "Weapons\Weapon.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Weapons\MissileLauncher.hpp"
#include "Weapons\LaserGun.hpp"
#include "Chassis\Chassis.hpp"
#include "Chassis\DefaultChassis.hpp"
#include "Chassis\SpeedChassis.hpp"

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