#include "Game/Entities/PlayerShip.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Pilots/PlayerPilot.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/Actives.hpp"
#include "Game/Items/Passives/Passive.hpp"

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip(PlayerPilot* pilot)
    : Ship((Pilot*)pilot)
    , m_healthText(new TextRenderable2D("HP:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_shieldText(new TextRenderable2D("SH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_speedText(new TextRenderable2D("MPH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
{
    m_isDead = false;
    
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_tintColor = GetPlayerColor(); 
    m_sprite->m_transform.SetScale(Vector2(0.25f, 0.25f));
    m_shipTrail->m_colorOverride = m_sprite->m_tintColor;
    InitializeUI();

    CalculateCollisionRadius();
    m_currentHp = CalculateHpValue();
    m_hitSoundMaxVolume = 1.0f;
    m_shieldSprite->m_tintColor = m_sprite->m_tintColor;

    if (g_nearlyInvulnerable)
    {
        m_currentHp = 99999999.0f;
    }
    if (g_spawnWithWeapon)
    {
        delete m_weapon;
        m_weapon = new MissileLauncher();
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::InitializeUI()
{
    m_speedometer = new Sprite("MuzzleFlash", TheGame::UI_LAYER);
    m_speedometer->m_tintColor = GetPlayerColor();
    m_speedometer->m_tintColor.SetAlphaFloat(0.75f);
    m_speedometer->m_transform.SetScale(Vector2(15.0f));
    m_speedometer->m_transform.SetPosition(Vector2(-0.5f, 0.5f));
    SpriteGameRenderer::instance->AnchorBottomRight(&m_speedometer->m_transform);

    m_healthText->m_color = RGBA::RED;
    m_shieldText->m_color = RGBA::CERULEAN;
    m_speedText->m_color = RGBA::GBDARKGREEN;
    m_healthText->m_transform.SetPosition(Vector2(-1.0f, 1.5f));
    m_shieldText->m_transform.SetPosition(Vector2(-1.0f, 1.0f));
    m_speedText->m_transform.SetPosition(Vector2(-1.0f, 0.5f));
    m_healthText->m_fontSize = 0.2f;
    m_shieldText->m_fontSize = 0.2f;
    m_speedText->m_fontSize = 0.2f;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_healthText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_speedText->m_transform);

    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);
    m_speedometer->m_viewableBy = visibilityFilter;
    m_healthText->m_viewableBy = visibilityFilter;
    m_shieldText->m_viewableBy = visibilityFilter;
    m_speedText->m_viewableBy = visibilityFilter;
}

//-----------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    //Casual reminder that the sprite is deleted on the entity
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_speedometer->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_healthText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_speedText->m_transform);
    delete m_healthText;
    delete m_shieldText;
    delete m_speedText;
    delete m_speedometer;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    if (m_isDead && m_pilot->m_inputMap.FindInputValue("Respawn")->WasJustPressed())
    {
        Respawn();
    }
    if (!m_isDead)
    {
        Ship::Update(deltaSeconds);
    }

    float speed = m_velocity.CalculateMagnitude();
    float rotationFromSpeed = 0.075f + (0.05f * speed);
    float newRotationDegrees = m_speedometer->m_transform.GetWorldRotationDegrees() + rotationFromSpeed;
    m_speedometer->m_transform.SetRotationDegrees(newRotationDegrees);

    m_healthText->m_text = Stringf("HP: %03i", static_cast<int>(m_currentHp * 10.0f));
    m_shieldText->m_text = Stringf("SH: %03i", static_cast<int>(m_currentShieldHealth * 10.0f));
    m_speedText->m_text = Stringf("MPH: %03i", static_cast<int>(speed * 10.0f));
}

//-----------------------------------------------------------------------------------
void PlayerShip::Render() const
{

}

//-----------------------------------------------------------------------------------
void PlayerShip::HideUI()
{
    m_healthText->Disable();
    m_shieldText->Disable();
    m_speedText->Disable();
    m_speedometer->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::ShowUI()
{
    m_healthText->Enable();
    m_shieldText->Enable();
    m_speedText->Enable();
    m_speedometer->Enable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::ResolveCollision(Entity* otherEntity)
{
    Ship::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void PlayerShip::Die()
{
    Ship::Die();
    DropPowerups();
    m_velocity = Vector2::ZERO;
    m_sprite->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::Respawn()
{
    m_isDead = false;
    Heal(CalculateHpValue());
    SetShieldHealth(CalculateShieldCapacityValue());
    m_velocity = Vector2::ZERO;
    m_shipTrail->Flush();
    m_sprite->Enable();
    SetPosition(TheGame::instance->m_currentGameMode->GetRandomPlayerSpawnPoint());
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropPowerups()
{
    unsigned int numPowerups = m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps();
    unsigned int numPowerupsToSpawn = 0;
    if (numPowerups <= 3)
    {
        numPowerupsToSpawn = numPowerups;
    }
    else
    {
        numPowerupsToSpawn = 3;
    }
    for (unsigned int i = 0; i < numPowerupsToSpawn; ++i)
    {
        DropRandomPowerup();
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropRandomPowerup()
{
    PowerUpType type;
    float* statValue = nullptr;
    do 
    {
        type = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::HYBRID));
        statValue = m_powerupStatModifiers.GetStatReference(type);
    } while (*statValue < 1.0f);

    TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(type), m_transform.GetWorldPosition());
    *statValue -= 1.0f;
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectWeapon()
{
    if (m_weapon)
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(m_weapon, m_transform.GetWorldPosition());
        m_weapon = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectChassis()
{
    if (m_chassis)
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(m_chassis, m_transform.GetWorldPosition());
        m_chassis = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectActive()
{
    if (m_activeEffect)
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(m_activeEffect, m_transform.GetWorldPosition());
        m_activeEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectPassive()
{
    if (m_passiveEffect)
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(m_passiveEffect, m_transform.GetWorldPosition());
        m_passiveEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------------
RGBA PlayerShip::GetPlayerColor()
{
    PlayerPilot* pilot = (PlayerPilot*)m_pilot;
    switch (pilot->m_playerNumber)
    {
    case 0:
        return RGBA::WHITE;
    case 1:
        return RGBA::GREEN;
    case 2:
        return RGBA::RED;
    case 3:
        return RGBA::CYAN;
    }
    ERROR_AND_DIE("Invalid Player number used to get a player color, have you spawned more than 4 players?");
}

//-----------------------------------------------------------------------------------
//The player now has ownership of this item pointer, and is responsible for cleanup.
void PlayerShip::PickUpItem(Item* pickedUpItem)
{
    if (!pickedUpItem)
    {
        return;
    }
    if (pickedUpItem->IsPowerUp())
    {
        PowerUp* powerUp = ((PowerUp*)pickedUpItem);
        powerUp->ApplyPickupEffect(this);

        if (powerUp->m_powerUpType == PowerUpType::HP)
        {
            Heal(Stats::HP_VALUE_PER_POINT);
        }

        GameMode::GetCurrent()->PlaySoundAt(powerUp->GetPickupSFXID(), GetPosition());
        ParticleSystem::PlayOneShotParticleEffect("PowerupPickup", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()), nullptr, powerUp->GetSpriteResource());

        delete powerUp;
    }
    if (pickedUpItem->IsWeapon())
    {
        EjectWeapon();
        m_weapon = (Weapon*)pickedUpItem;
    }
    if (pickedUpItem->IsChassis())
    {
        EjectChassis();
        m_chassis = (Chassis*)pickedUpItem;
    }
}
