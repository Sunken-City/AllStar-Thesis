#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Projectiles/Missile.hpp"

//-----------------------------------------------------------------------------------
MissileLauncher::MissileLauncher()
{
    m_statBonuses.shotHoming = 30.0f;
    m_statBonuses.rateOfFire = -4.0f;
    m_statBonuses.damage = -2.0f;
    m_name = "Missile Launcher";
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
    static SoundID bulletSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    bool successfullyFired = false;
    float secondsPerWeaponFire = 1.0f / shooter->CalculateRateOfFireValue();

    if (shooter->m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
    {
        GameMode* currentGameMode = TheGame::instance->m_currentGameMode;

        for (unsigned int i = 0; i < m_numMisslesPerShot; i++)
        {
            Projectile* bullet = (Projectile*)new Missile(shooter, MathUtils::GetRandom(-m_spreadDegrees, m_spreadDegrees), shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());
            if (shooter->IsPlayer())
            {
                bullet->m_reportDPSToPlayer = true;
            }
            currentGameMode->SpawnBullet(bullet);
        }

        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;

        Vector2 shotPosition = shooter->GetMuzzlePosition();
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, 0.5f);
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::BULLET_LAYER, Transform2D(shotPosition));
    }
    return successfullyFired;
}
