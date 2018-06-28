#include "Game/Items/Weapons/LeftHookGun.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Projectiles/LeftHookLaser.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
LeftHookGun::LeftHookGun()
    : Weapon()
{
    m_name = "Left Hook Blaster";

    m_spreadDegrees = 10.0f;
    m_knockbackPerBullet = LeftHookLaser::KNOCKBACK_MAGNITUDE;
}

//-----------------------------------------------------------------------------------
LeftHookGun::~LeftHookGun()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* LeftHookGun::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("LeftHookGun");
}

//-----------------------------------------------------------------------------------
bool LeftHookGun::AttemptFire(Ship* shooter)
{
    static float RUMBLE_PERCENTAGE = 0.1f;
    static float SECONDS_TO_RUMBLE = 0.075f;
    static const SoundID bulletSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    bool successfullyFired = false;
    float secondsPerWeaponFire = 1.0f / shooter->CalculateRateOfFireValue();

    if (shooter->m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
    {
        GameMode* currentGameMode = TheGame::instance->m_currentGameMode;

        float halfSpreadDegrees = m_spreadDegrees / 2.0f;
        float degreesOffset = MathUtils::GetRandomFloat(-halfSpreadDegrees, halfSpreadDegrees);
        Projectile* bullet = (Projectile*)new LeftHookLaser(shooter, degreesOffset, shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());

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
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, TheGame::BULLET_VOLUME, MathUtils::GetRandomFloat(0.9f, 1.1f));
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::BULLET_LAYER_BLOOM, Transform2D(shotPosition));
    }
    return successfullyFired;
}

