#include "Game/Items/Weapons/LaserGun.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Projectiles/Laser.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
LaserGun::LaserGun()
    : Weapon()
{
    m_name = "Laser Blaster";

    m_spreadDegrees = 10.0f;
    m_knockbackPerBullet = Laser::KNOCKBACK_MAGNITUDE;
}

//-----------------------------------------------------------------------------------
LaserGun::~LaserGun()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* LaserGun::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("DefaultWeapon");
}

//-----------------------------------------------------------------------------------
bool LaserGun::AttemptFire(Ship* shooter)
{
    static const SoundID bulletSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    static float RUMBLE_PERCENTAGE = 0.1f;
    static float SECONDS_TO_RUMBLE = 0.075f;
    bool successfullyFired = false;
    float secondsPerWeaponFire = 1.0f / shooter->CalculateRateOfFireValue();

    if (shooter->m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
    {
        GameMode* currentGameMode = TheGame::instance->m_currentGameMode;

        float halfSpreadDegrees = m_spreadDegrees / 2.0f;
        float degreesOffset = MathUtils::GetRandomFloat(-halfSpreadDegrees, halfSpreadDegrees);
        Projectile* bullet = (Projectile*)new Laser(shooter, degreesOffset, shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());

        if (shooter->IsPlayer())
        {
            bullet->m_reportDPSToPlayer = true;
        }
        if (shooter->m_pilot)
        {
            shooter->m_pilot->LightRumble(RUMBLE_PERCENTAGE, SECONDS_TO_RUMBLE);
            shooter->m_pilot->RecoilScreenshake(0.0f, -0.05f * Vector2::DegreesToDirection(-shooter->m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP));
        }
        currentGameMode->SpawnBullet(bullet);
        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;

        Vector2 shotPosition = shooter->GetMuzzlePosition();
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, 0.5f);
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::BULLET_LAYER_BLOOM, Transform2D(shotPosition));
    }
    return successfullyFired;
}

