#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/TheGame.hpp"

//-----------------------------------------------------------------------------------
MissileLauncher::MissileLauncher()
{
    m_statBonuses.shotHoming = 10.0f;
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
    bool successfullyFired = false;
    float secondsPerWeaponFire = 1.0f / shooter->CalculateRateOfFireValue();

    if (shooter->m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
    {
        TheGame::instance->m_currentGameMode->SpawnBullet(shooter);
        shooter->m_secondsSinceLastFiredWeapon = 0.0f;
        successfullyFired = true;
    }
    return successfullyFired;
}
