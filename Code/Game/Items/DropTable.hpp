#pragma once
#include "Weapons\Weapon.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Weapons\MissileLauncher.hpp"
#include "Weapons\LaserGun.hpp"
#include "Chassis\Chassis.hpp"
#include "Chassis\AttractorChassis.hpp"
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
#include "Weapons\SpreadShot.hpp"
#include "Passives\SprayAndPrayPassive.hpp"
#include "Passives\SharpshooterPassive.hpp"
#include "Weapons\WaveGun.hpp"
#include "Actives\ReflectorActive.hpp"

//-----------------------------------------------------------------------------------
Weapon* GetRandomWeapon()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(3);
    if (randomNumber == 0)
    {
        return new MissileLauncher();
    }
    else if (randomNumber == 1)
    {
        return new WaveGun();
    }
    else
    {
        return new SpreadShot();
    }
}

//-----------------------------------------------------------------------------------
Chassis* GetRandomChassis()
{
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(3);
    if (randomNumber == 0)
    {
        return new SpeedChassis();
    }
    else if (randomNumber == 1)
    {
        return new AttractorChassis();
    }
    else if (randomNumber == 2)
    {
        return new TankChassis();
    }
    else if (randomNumber == 3)
    {
        return new PowerChassis();
    }
    else
    {
        return new GlassCannonChassis();
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
        return new ShieldActive();
    }
    else if (randomNumber == 2)
    {
        return new QuickshotActive();
    }
    else if (randomNumber == 3)
    {
        return new ReflectorActive();
    }
    else
    {
        return new BoostActive();
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
        return new SharpshooterPassive();
    }
    else if (randomNumber == 2)
    {
        return new SprayAndPrayPassive();
    }
    else
    {
        return new SpecialTrailPassive();
    }
}