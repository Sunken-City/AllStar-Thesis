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
#include "Game/Items/Actives/ActiveEffect.hpp"
#include "Game/Items/Passives/PassiveEffect.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "TextSplash.hpp"
#include "Engine/Time/Time.hpp"
#include "../Items/Weapons/LaserGun.hpp"
#include "../Items/Chassis/SpeedChassis.hpp"
#include "../Items/Passives/CloakPassive.hpp"
#include "../Items/Actives/TeleportActive.hpp"

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip(PlayerPilot* pilot)
    : Ship((Pilot*)pilot)
    , m_healthText(new TextRenderable2D("HP:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_shieldText(new TextRenderable2D("SH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_speedText(new TextRenderable2D("MPH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_dpsText(new TextRenderable2D("DPS:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_recolorShader(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/recolorable2D.frag"))
{
    m_isDead = false;
    m_recolorMaterial = new Material(m_recolorShader, SpriteGameRenderer::instance->m_defaultRenderState);
    
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_material = m_recolorMaterial;
    m_sprite->m_recolorMode = (SpriteRecolorMode)(((PlayerPilot*)m_pilot)->m_playerNumber + 4);
    m_sprite->m_transform.SetScale(Vector2(0.25f, 0.25f));
    m_shipTrail->m_colorOverride = GetPlayerColor();
    InitializeUI();
    InitializeStatGraph();

    CalculateCollisionRadius();
    m_currentHp = CalculateHpValue();
    m_hitSoundMaxVolume = 1.0f;
    m_shieldSprite->m_tintColor = GetPlayerColor();

    if (g_nearlyInvulnerable)
    {
        m_currentHp = 99999999.0f;
    }
    if (g_spawnWithDebugLoadout)
    {
        PickUpItem(new MissileLauncher());
        PickUpItem(new SpeedChassis());
        PickUpItem(new TeleportActive());
        PickUpItem(new CloakPassive());
    }
    m_equipUI->Disable();
    m_currentWeaponUI->Disable();
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

    m_equipUI = new Sprite("MuzzleFlash", TheGame::UI_LAYER);
    m_equipUI->m_tintColor = GetPlayerColor();
    m_equipUI->m_tintColor.SetAlphaFloat(0.75f);
    m_equipUI->m_transform.SetScale(Vector2(15.0f));
    m_equipUI->m_transform.SetPosition(Vector2(0.5f, 0.5f));
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_equipUI->m_transform);

    m_currentWeaponUI = new Sprite("DefaultWeapon", TheGame::UI_LAYER);
    m_currentWeaponUI->m_tintColor = RGBA::GREEN;
    m_currentWeaponUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentWeaponUI->m_transform.SetScale(Vector2(0.5f));
    m_currentWeaponUI->m_transform.SetPosition(Vector2(0.5f, 0.5f));
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_currentWeaponUI->m_transform);

    m_healthText->m_color = RGBA::RED;
    m_shieldText->m_color = RGBA::CERULEAN;
    m_speedText->m_color = RGBA::GBDARKGREEN;
    m_dpsText->m_color = RGBA::GBLIGHTGREEN;
    m_healthText->m_transform.SetPosition(Vector2(-1.0f, 1.8f));
    m_shieldText->m_transform.SetPosition(Vector2(-1.0f, 1.3f));
    m_speedText->m_transform.SetPosition(Vector2(-1.0f, 0.8f));
    m_dpsText->m_transform.SetPosition(Vector2(-1.0f, 0.3f));
    m_healthText->m_fontSize = 0.1f;
    m_shieldText->m_fontSize = 0.1f;
    m_speedText->m_fontSize = 0.1f;
    m_dpsText->m_fontSize = 0.1f;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_healthText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_speedText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_dpsText->m_transform);

    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);
    m_speedometer->m_viewableBy = visibilityFilter;
    m_equipUI->m_viewableBy = visibilityFilter;
    m_currentWeaponUI->m_viewableBy = visibilityFilter;
    m_healthText->m_viewableBy = visibilityFilter;
    m_shieldText->m_viewableBy = visibilityFilter;
    m_speedText->m_viewableBy = visibilityFilter;
    m_dpsText->m_viewableBy = visibilityFilter;
}

//-----------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    //Casual reminder that the sprite is deleted on the entity
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_speedometer->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_equipUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_currentWeaponUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_healthText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_speedText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_dpsText->m_transform);
    delete m_speedometer;
    delete m_equipUI;
    delete m_currentWeaponUI;
    delete m_healthText;
    delete m_shieldText;
    delete m_speedText;
    delete m_dpsText;

    delete m_statValuesBG;
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        delete m_statValues[i];
    }

    delete m_recolorShader;
    delete m_recolorMaterial;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    static float timeOfLastReset = 0.0f;
    if (m_isDead && m_pilot->m_inputMap.FindInputValue("Respawn")->WasJustPressed())
    {
        Respawn();
    }
    if (!m_isDead)
    {
        Ship::Update(deltaSeconds);
    }

    if (InputSystem::instance->WasKeyJustPressed('R'))
    {
        timeOfLastReset = m_age - deltaSeconds;
        m_totalDamageDone = 0.0f;
    }

    CheckToEjectEquipment(deltaSeconds);
    UpdateEquips(deltaSeconds);

    float dps = (m_totalDamageDone) / (m_age - timeOfLastReset);
    float speed = m_velocity.CalculateMagnitude();
    float rotationFromSpeed = 0.075f + (0.05f * speed);
    float newRotationDegrees = m_speedometer->m_transform.GetWorldRotationDegrees() + rotationFromSpeed;
    m_speedometer->m_transform.SetRotationDegrees(newRotationDegrees);
    m_equipUI->m_transform.SetRotationDegrees(-newRotationDegrees);
    m_currentWeaponUI->m_spriteResource = m_weapon ? m_weapon->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("Shield");

    m_healthText->m_text = Stringf("HP: %03i", static_cast<int>(m_currentHp));
    m_shieldText->m_text = Stringf("SH: %03i", static_cast<int>(m_currentShieldHealth));
    m_speedText->m_text = Stringf("MPH: %03i", static_cast<int>(speed * 10.0f));
    m_dpsText->m_text = Stringf("DPS: %03i", static_cast<int>(dps));
}

//-----------------------------------------------------------------------------------
void PlayerShip::UpdateEquips(float deltaSeconds)
{
    if (m_passiveEffect)
    {
        m_passiveEffect->Update(deltaSeconds);
    }
    if (m_activeEffect)
    {
        if (m_pilot->m_inputMap.WasJustPressed("Activate"))
        {
            NamedProperties props;
            props.Set<Ship*>("ShipPtr", (Ship*)this);
            m_activeEffect->Activate(props);
        }
        m_activeEffect->Update(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::Render() const
{

}

//-----------------------------------------------------------------------------------
void PlayerShip::HideUI()
{
    m_speedometer->Disable();
    m_equipUI->Disable();
    m_currentWeaponUI->Disable();
    m_healthText->Disable();
    m_shieldText->Disable();
    m_speedText->Disable();
    m_dpsText->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::ShowUI()
{
    m_speedometer->Enable();
    m_equipUI->Enable();
    m_currentWeaponUI->Enable();
    m_healthText->Enable();
    m_shieldText->Enable();
    m_speedText->Enable();
    m_dpsText->Enable();
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
    if (GameMode::GetCurrent()->m_dropItemsOnDeath)
    {
        DropPowerups();
    }
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
    if (!dynamic_cast<LaserGun*>(m_weapon))
    {
        EjectWeapon();
        m_weapon = new LaserGun();
    }

    unsigned int numPowerups = m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps();
    unsigned int numPowerupsToDrop = (unsigned int)(numPowerups * 0.2f);
    unsigned int numPowerupsToSpawn = (numPowerups <= 3) ? numPowerups : numPowerupsToDrop;

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
        type = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::NUM_POWERUP_TYPES));
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
        TheGame::instance->m_currentGameMode->SpawnPickup(m_weapon, m_transform.GetWorldPosition() - (Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees()) * 0.5f));
        m_weapon = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectChassis()
{
    if (m_chassis)
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(m_chassis, m_transform.GetWorldPosition() - (Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees()) * 0.5f));
        m_chassis = nullptr;
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("PlayerShip");
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectActive()
{
    if (m_activeEffect)
    {
        if (m_activeEffect->IsActive())
        {
            m_activeEffect->Deactivate(NamedProperties::NONE);
        }
        TheGame::instance->m_currentGameMode->SpawnPickup(m_activeEffect, m_transform.GetWorldPosition() - (Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees()) * 0.5f));
        m_activeEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::EjectPassive()
{
    if (m_passiveEffect)
    {
        m_passiveEffect->Deactivate(NamedProperties::NONE);
        TheGame::instance->m_currentGameMode->SpawnPickup(m_passiveEffect, m_transform.GetWorldPosition() - (Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees()) * 0.5f));
        m_passiveEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::InitializeStatGraph()
{
    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);

    m_statValuesBG = new Sprite("Quad", TheGame::STAT_GRAPH_LAYER_BACKGROUND, true);
    m_statValuesBG->m_transform.SetScale(Vector2(8.0f, 11.0f));
    m_statValuesBG->m_tintColor = RGBA::GBDARKGREEN;
    m_statValuesBG->m_transform.SetPosition(Vector2::ZERO);
    m_statValuesBG->m_viewableBy = visibilityFilter;
    m_statValuesBG->Disable();

    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        TextRenderable2D* statLine = new TextRenderable2D(Stringf("%s: %i", PowerUp::GetPowerUpSpriteResourceName((PowerUpType)i), 0), Transform2D(Vector2(0.0f, 3.0f - (0.5f * i))), TheGame::STAT_GRAPH_LAYER);
        statLine->m_fontSize = 0.3f;
        statLine->Disable();
        statLine->m_viewableBy = visibilityFilter;
        m_statValues[i] = statLine;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::ShowStatGraph()
{
    m_statValuesBG->Enable();
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        PowerUpType type = (PowerUpType)i;
        m_statValues[i]->m_text = Stringf("%s : %i", PowerUp::GetPowerUpSpriteResourceName(type), static_cast<int>(*m_powerupStatModifiers.GetStatReference(type)));
        m_statValues[i]->Enable();
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::HideStatGraph()
{
    m_statValuesBG->Disable();
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        m_statValues[i]->Disable();
    }
}

//-----------------------------------------------------------------------------------
RGBA PlayerShip::GetPlayerColor()
{
    PlayerPilot* pilot = (PlayerPilot*)m_pilot;
    switch (pilot->m_playerNumber)
    {
    case 0:
        return RGBA::VAPORWAVE;
    case 1:
        return RGBA::CYAN;
    case 2:
        return RGBA::GREEN;
    case 3:
        return RGBA::YELLOW;
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

        float randomDegrees = MathUtils::GetRandom(-80.0f, 80.0f);
        Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;
        TextSplash::CreateTextSplash(Stringf("%s Up", powerUp->GetPowerUpSpriteResourceName()), m_transform, velocity, RGBA::GBLIGHTGREEN);

        delete powerUp;
    }
    else if (pickedUpItem->IsWeapon())
    {
        EjectWeapon();
        m_weapon = (Weapon*)pickedUpItem;
    }
    else if (pickedUpItem->IsChassis())
    {
        EjectChassis();
        m_chassis = (Chassis*)pickedUpItem;
        m_sprite->m_spriteResource = m_chassis->GetShipSpriteResource();
    }
    else if (pickedUpItem->IsPassiveEffect())
    {
        if (m_passiveEffect)
        {
            EjectPassive();
        }
        m_passiveEffect = (PassiveEffect*)pickedUpItem;
        NamedProperties props;
        props.Set<Ship*>("ShipPtr", (Ship*)this);
        m_passiveEffect->Activate(props);
    }
    else if (pickedUpItem->IsActiveEffect())
    {
        if (m_activeEffect)
        {
            EjectActive();
        }
        m_activeEffect = (ActiveEffect*)pickedUpItem;
    }
}

//-----------------------------------------------------------------------------------
bool PlayerShip::CanPickUp(Item* item)
{
    if (item->IsWeapon() && m_weapon == nullptr)
    {
        return true;
    }
    else if (item->IsChassis() && m_chassis == nullptr)
    {
        return true;
    }
    else if (item->IsPassiveEffect() && m_passiveEffect == nullptr)
    {
        return true;
    }
    else if (item->IsActiveEffect() && m_activeEffect == nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::CheckToEjectEquipment(float)
{
    static const double EJECT_TIME_SECONDS = 1.0f;
    static const double EJECT_TIME_MILLISECONDS = EJECT_TIME_SECONDS * 1000.0f;
    double currentTimeMilliseconds = GetCurrentTimeMilliseconds();

    if (m_pilot->m_inputMap.WasJustPressed("EjectActive"))
    {
        m_activeBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (m_pilot->m_inputMap.WasJustPressed("EjectPassive"))
    {
        m_passiveBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (m_pilot->m_inputMap.WasJustPressed("EjectWeapon"))
    {
        m_weaponBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (m_pilot->m_inputMap.WasJustPressed("EjectChassis"))
    {
        m_chassisBeginEjectMilliseconds = currentTimeMilliseconds;
    }

    if (currentTimeMilliseconds - m_activeBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS && m_pilot->m_inputMap.IsDown("EjectActive"))
    {
        EjectActive();
        m_activeBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (currentTimeMilliseconds - m_passiveBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS && m_pilot->m_inputMap.IsDown("EjectPassive"))
    {
        EjectPassive();
        m_passiveBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (currentTimeMilliseconds - m_weaponBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS && m_pilot->m_inputMap.IsDown("EjectWeapon"))
    {
        EjectWeapon();
        m_weaponBeginEjectMilliseconds = currentTimeMilliseconds;
    }
    if (currentTimeMilliseconds - m_chassisBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS && m_pilot->m_inputMap.IsDown("EjectChassis"))
    {
        EjectChassis();
        m_chassisBeginEjectMilliseconds = currentTimeMilliseconds;
    }
}
