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
    m_statBonuses.shotHoming = m_shotHomingBonus;
    m_statBonuses.rateOfFire = m_rateOfFireBonus;
}

//-----------------------------------------------------------------------------------
MissileLauncher::~MissileLauncher()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* MissileLauncher::GetSpriteResource()
{
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
            Projectile* bullet = (Projectile*)new Missile(shooter, MathUtils::GetRandom(-45.0f, 45.0f), shooter->CalculateDamageValue(), shooter->CalculateShieldDisruptionValue(), shooter->CalculateShotHomingValue());
            currentGameMode->SpawnBullet(bullet);
        }

        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;

        Vector2 shotPosition = shooter->GetMuzzlePosition();
        currentGameMode->PlaySoundAt(bulletSound, shotPosition, 0.5f);
        ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::PLAYER_BULLET_LAYER, Transform2D(shotPosition));
    }
    return successfullyFired;
}
