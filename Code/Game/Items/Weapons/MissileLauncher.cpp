#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Projectiles/Missile.hpp"

//-----------------------------------------------------------------------------------
MissileLauncher::MissileLauncher()
{
    m_name = "Missile Launcher";

    m_statBonuses.shotHoming = 10.0f;
    m_statBonuses.rateOfFire = -2.0f;
    m_statBonuses.damage = -4.0f;

    m_numProjectilesPerShot = 1;
    m_spreadDegrees = 70.0f;
    m_knockbackPerBullet = Missile::KNOCKBACK_MAGNITUDE;
}

//-----------------------------------------------------------------------------------
MissileLauncher::~MissileLauncher()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* MissileLauncher::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("MissileLauncher");
}

//-----------------------------------------------------------------------------------
bool MissileLauncher::AttemptFire(Ship* shooter)
{
    static float RUMBLE_PERCENTAGE = 0.1f;
    static float SECONDS_TO_RUMBLE = 0.075f;
    static const int NUM_SOUNDS = 1;
    static const SoundID bulletSounds[NUM_SOUNDS] = {
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/missileFire.ogg")
    };

    SoundID bulletSound = bulletSounds[MathUtils::GetRandomIntFromZeroTo(NUM_SOUNDS)];
    bool successfullyFired = false;
    float secondsPerWeaponFire = 1.0f / shooter->CalculateRateOfFireValue();

    if (shooter->m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
    {
        GameMode* currentGameMode = TheGame::instance->m_currentGameMode;

        for (unsigned int i = 0; i < m_numProjectilesPerShot; i++)
        {
            float halfSpreadDegrees = m_spreadDegrees / 2.0f;
            float degreesOffset = MathUtils::GetRandomFloat(-halfSpreadDegrees, halfSpreadDegrees);
            Projectile* bullet = (Projectile*)new Missile(shooter, degreesOffset, shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());
            if (shooter->IsPlayer())
            {
                bullet->m_reportDPSToPlayer = true;
            }
            currentGameMode->SpawnBullet(bullet);
        }
        if (shooter->m_pilot)
        {
            shooter->m_pilot->LightRumble(RUMBLE_PERCENTAGE, SECONDS_TO_RUMBLE);
            shooter->m_pilot->RecoilScreenshake(0.0f, -0.05f * Vector2::DegreesToDirection(shooter->m_transform.GetWorldRotationDegrees()));
        }

        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;

        Vector2 shotPosition = shooter->GetMuzzlePosition();
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, TheGame::BULLET_VOLUME, MathUtils::GetRandomFloat(0.9f, 1.1f));
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::BULLET_LAYER_BLOOM, Transform2D(shotPosition));
    }
    return successfullyFired;
}
