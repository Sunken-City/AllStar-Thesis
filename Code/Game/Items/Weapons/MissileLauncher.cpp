#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"

//-----------------------------------------------------------------------------------
MissileLauncher::MissileLauncher()
{
    m_statBonuses.shotHoming = 20.0f;
    m_statBonuses.rateOfFire = -3.0f;
}

//-----------------------------------------------------------------------------------
MissileLauncher::~MissileLauncher()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* MissileLauncher::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("Placeholder");
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

        Projectile* bullet = new Projectile(shooter, shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());
        currentGameMode->SpawnBullet(bullet);
        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;

        Vector2 shotPosition = shooter->GetMuzzlePosition();
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, 0.5f);
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::PLAYER_BULLET_LAYER, Transform2D(shotPosition));
    }
    return successfullyFired;
}
