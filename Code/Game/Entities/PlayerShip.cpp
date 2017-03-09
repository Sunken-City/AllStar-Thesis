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
#include "Game/Items/Weapons/LaserGun.hpp"
#include "Game/Items/Chassis/SpeedChassis.hpp"
#include "Game/Items/Passives/CloakPassive.hpp"
#include "Game/Items/Actives/WarpActive.hpp"
#include "../Items/Chassis/BlackHoleChassis.hpp"
#include "../Items/Passives/SpecialTrailPassive.hpp"
#include "../Items/Actives/ShieldActive.hpp"
#include "../Items/Actives/BoostActive.hpp"
#include "../Items/Chassis/TankChassis.hpp"
#include "../Items/Weapons/SpreadShot.hpp"
#include "../Items/Passives/SprayAndPrayPassive.hpp"
#include "../Items/Weapons/WaveGun.hpp"

const Vector2 PlayerShip::DEFAULT_SCALE = Vector2(2.0f);
const char* PlayerShip::RESPAWN_TEXT = "Press Start to Respawn";
const char* PlayerShip::DEAD_TEXT = "You have Died";

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip(PlayerPilot* pilot)
    : Ship((Pilot*)pilot)
    , m_respawnText(new TextRenderable2D(RESPAWN_TEXT, Transform2D(Vector2(0.0f, 0.0f)), TheGame::FBO_FREE_TEXT_LAYER, false))
    , m_healthText(new TextRenderable2D("HP:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_shieldText(new TextRenderable2D("SH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_speedText(new TextRenderable2D("MPH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_scoreText(new TextRenderable2D("DPS:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_paletteSwapShader(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/paletteSwap2D.frag"))
    , m_cooldownShader(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/cooldown.frag"))
{
    m_isDead = false;
    m_paletteSwapMaterial = new Material(m_paletteSwapShader, SpriteGameRenderer::instance->m_defaultRenderState);
    m_cooldownMaterial = new Material(m_cooldownShader, SpriteGameRenderer::instance->m_defaultRenderState);

    m_sprite = new Sprite("DefaultChassis", TheGame::PLAYER_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    m_sprite->m_material = m_paletteSwapMaterial;
    float paletteIndex = ((float)((PlayerPilot*)m_pilot)->m_playerNumber + 1.0f) / 16.0f;
    m_sprite->m_material->SetFloatUniform("PaletteOffset", paletteIndex);
    m_sprite->m_recolorMode = (SpriteRecolorMode)(((PlayerPilot*)m_pilot)->m_playerNumber + 4);
    m_sprite->m_material->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ColorPalettes")->m_texture);
    m_transform.SetScale(DEFAULT_SCALE);

    m_shieldSprite->m_material = m_sprite->m_material;
    m_shipTrail->m_colorOverride = GetPlayerColor();
    m_factionColor = GetPlayerColor();
    InitializeUI();
    InitializeStatGraph();

    SetVortexShaderPosition(Vector2(-999.0f)); //If we don't reset this value somewhere, the player could get warped around holes that don't exist anymore in minigames.
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
        PickUpItem(new WaveGun());
        PickUpItem(new SpeedChassis());
        PickUpItem(new BoostActive());
        PickUpItem(new SprayAndPrayPassive());
    }

    m_shieldDownEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\shieldDown.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );
    SpriteGameRenderer::instance->AddEffectToLayer(m_shieldDownEffect, TheGame::FULL_SCREEN_EFFECT_LAYER, SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber));
}

//-----------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    //Casual reminder that the sprite is deleted on the entity
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_equipUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_currentWeaponUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_currentActiveUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_currentChassisUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_currentPassiveUI->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_playerData->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_healthText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_speedText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_scoreText->m_transform);
    delete m_equipUI;
    delete m_playerData;
    delete m_currentWeaponUI;
    delete m_currentActiveUI;
    delete m_currentChassisUI;
    delete m_currentPassiveUI;
    delete m_respawnText;
    delete m_healthText;
    delete m_shieldText;
    delete m_speedText;
    delete m_scoreText;
    delete m_shieldDownEffect->m_shaderProgram;
    delete m_shieldDownEffect;

    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_shieldDownEffect, TheGame::FULL_SCREEN_EFFECT_LAYER);

    delete m_statValuesBG;
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        delete m_statValues[i];
    }

    delete m_paletteSwapShader;
    delete m_paletteSwapMaterial;

    delete m_cooldownShader;
    delete m_cooldownMaterial;
}

//-----------------------------------------------------------------------------------
void PlayerShip::InitializeUI()
{
    m_equipUI = new Sprite("MuzzleFlash", TheGame::UI_LAYER);
    m_equipUI->m_tintColor = GetPlayerColor();
    m_equipUI->m_tintColor.SetAlphaFloat(0.75f);
    m_equipUI->m_transform.SetScale(Vector2(15.0f));
    m_equipUI->m_transform.SetPosition(Vector2(-0.5f, 0.5f));
    SpriteGameRenderer::instance->AnchorBottomRight(&m_equipUI->m_transform);

    m_playerData = new Sprite("MuzzleFlash", TheGame::UI_LAYER);
    m_playerData->m_tintColor = GetPlayerColor();
    m_playerData->m_tintColor.SetAlphaFloat(0.75f);
    m_playerData->m_transform.SetScale(Vector2(15.0f));
    m_playerData->m_transform.SetPosition(Vector2(0.5f, 0.5f));
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_playerData->m_transform);

    m_currentWeaponUI = new Sprite("EmptyEquipSlot", TheGame::UI_LAYER);
    m_currentWeaponUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentWeaponUI->m_transform.SetScale(Vector2(1.0f));
    m_currentWeaponUI->m_transform.SetPosition(Vector2(-0.4f, 1.0f));
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentWeaponUI->m_transform);

    m_currentActiveUI = new Sprite("EmptyEquipSlot", TheGame::UI_LAYER);
    m_currentActiveUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentActiveUI->m_transform.SetScale(Vector2(1.0f));
    m_currentActiveUI->m_transform.SetPosition(Vector2(-1.0f, 0.4f));
    m_currentActiveUI->m_material = m_cooldownMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentActiveUI->m_transform);

    m_currentChassisUI = new Sprite("EmptyEquipSlot", TheGame::UI_LAYER);
    m_currentChassisUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentChassisUI->m_transform.SetScale(Vector2(1.0f));
    m_currentChassisUI->m_transform.SetPosition(Vector2(-1.0f, 1.6f));
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentChassisUI->m_transform);

    m_currentPassiveUI = new Sprite("EmptyEquipSlot", TheGame::UI_LAYER);
    m_currentPassiveUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentPassiveUI->m_transform.SetScale(Vector2(1.0f));
    m_currentPassiveUI->m_transform.SetPosition(Vector2(-1.6f, 1.0f));
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentPassiveUI->m_transform);
          
    m_respawnText->m_color = RGBA::WHITE;
    m_healthText->m_color = RGBA::RED;
    m_shieldText->m_color = RGBA::CERULEAN;
    m_speedText->m_color = RGBA::GBDARKGREEN;
    m_scoreText->m_color = RGBA::GBLIGHTGREEN;
    m_healthText->m_transform.SetPosition(Vector2(1.0f, 1.8f));
    m_shieldText->m_transform.SetPosition(Vector2(1.0f, 1.3f));
    m_speedText->m_transform.SetPosition(Vector2(1.1f, 0.8f));
    m_scoreText->m_transform.SetPosition(Vector2(1.1f, 0.3f));
    m_respawnText->m_transform.SetScale(Vector2(2.0f));
    m_healthText->m_transform.SetScale(Vector2(2.0f));
    m_shieldText->m_transform.SetScale(Vector2(2.0f));
    m_speedText->m_transform.SetScale(Vector2(2.0f));
    m_scoreText->m_transform.SetScale(Vector2(2.0f));
    m_respawnText->m_fontSize = 0.1f;
    m_healthText->m_fontSize = 0.1f;
    m_shieldText->m_fontSize = 0.1f;
    m_speedText->m_fontSize = 0.1f;
    m_scoreText->m_fontSize = 0.1f;
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_healthText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_speedText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_scoreText->m_transform);

    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);
    m_equipUI->m_viewableBy = visibilityFilter;
    m_playerData->m_viewableBy = visibilityFilter;
    m_currentWeaponUI->m_viewableBy = visibilityFilter;
    m_currentChassisUI->m_viewableBy = visibilityFilter;
    m_currentActiveUI->m_viewableBy = visibilityFilter;
    m_currentPassiveUI->m_viewableBy = visibilityFilter;
    m_respawnText->m_viewableBy = visibilityFilter;
    m_healthText->m_viewableBy = visibilityFilter;
    m_shieldText->m_viewableBy = visibilityFilter;
    m_speedText->m_viewableBy = visibilityFilter;
    m_scoreText->m_viewableBy = visibilityFilter;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    if (m_isDead)
    {
        m_shieldDownEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        if (m_pilot->m_inputMap.FindInputValue("Respawn")->WasJustPressed() && (GameMode::GetCurrent()->m_respawnAllowed))
        {
            Respawn();
        }
    }
    if (!m_isDead)
    {
        Ship::Update(deltaSeconds);
    }

    CheckToEjectEquipment(deltaSeconds);
    UpdateEquips(deltaSeconds);
    UpdatePlayerUI(deltaSeconds);
    DebugUpdate(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void PlayerShip::UpdatePlayerUI(float deltaSeconds)
{
    float speed = m_velocity.CalculateMagnitude();
    float rotationFromSpeed = 0.075f + (0.05f * speed);
    float newRotationDegrees = m_equipUI->m_transform.GetWorldRotationDegrees() + rotationFromSpeed;
    m_equipUI->m_transform.SetRotationDegrees(newRotationDegrees);
    m_playerData->m_transform.SetRotationDegrees(-newRotationDegrees);

    m_currentWeaponUI->m_spriteResource = m_weapon ? m_weapon->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyEquipSlot");
    m_currentActiveUI->m_spriteResource = m_activeEffect ? m_activeEffect->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyEquipSlot");
    m_currentChassisUI->m_spriteResource = m_chassis ? m_chassis->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyEquipSlot");
    m_currentPassiveUI->m_spriteResource = m_passiveEffect ? m_passiveEffect->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyEquipSlot");

    m_healthText->m_text = Stringf("HP: %03i", static_cast<int>(m_currentHp));
    m_shieldText->m_text = Stringf("SH: %03i", static_cast<int>(m_currentShieldHealth));
    m_speedText->m_text = Stringf("MPH: %03i", static_cast<int>((speed / CalculateTopSpeedValue()) * 100.0f));
    m_scoreText->m_text = Stringf("LVL: %03i", m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps());

    if (m_activeEffect)
    {
        m_cooldownMaterial->SetFloatUniform("gPercentage", m_activeEffect->m_energy);
    }
    else
    {
        m_cooldownMaterial->SetFloatUniform("gPercentage", 1.0f);
    }
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
        if (m_pilot->m_inputMap.WasJustPressed("Activate") && IsAlive())
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
    m_equipUI->Disable();
    m_playerData->Disable();
    m_currentWeaponUI->Disable();
    m_currentChassisUI->Disable();
    m_currentActiveUI->Disable();
    m_currentPassiveUI->Disable();
    m_respawnText->Disable();
    m_healthText->Disable();
    m_shieldText->Disable();
    m_speedText->Disable();
    m_scoreText->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::ShowUI()
{
    m_equipUI->Enable();
    m_playerData->Enable();
    m_currentWeaponUI->Enable();
    m_currentChassisUI->Enable();
    m_currentActiveUI->Enable();
    m_currentPassiveUI->Enable();
    m_healthText->Enable();
    m_shieldText->Enable();
    m_speedText->Enable();
    m_scoreText->Enable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::ResolveCollision(Entity* otherEntity)
{
    Ship::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
float PlayerShip::TakeDamage(float damage, float disruption /*= 1.0f*/)
{
    bool hadShield = HasShield();
    float ratioOfDamage = damage / CalculateHpValue();
    float returnValue = Ship::TakeDamage(damage, disruption);

    if (!HasShield())
    {
        if (hadShield)
        {
            m_shieldDownEffect->SetFloatUniform("gEffectTime", (float)GetCurrentTimeSeconds());
        }
        ratioOfDamage *= 4.0f;
        ratioOfDamage = Clamp(ratioOfDamage, 0.0f, 1.0f);
        m_pilot->LightRumble(ratioOfDamage, 0.25f);
    }
    m_pilot->HeavyRumble(ratioOfDamage, 0.25f);

    return returnValue;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Die()
{
    Ship::Die();
    if (GameMode::GetCurrent()->m_dropItemsOnDeath)
    {
        DropPowerups();
    }

    GameMode::GetCurrent()->RecordPlayerDeath(this);
    SpriteGameRenderer::instance->AddScreenshakeMagnitude(0.4f, Vector2::ZERO, ((PlayerPilot*)m_pilot)->m_playerNumber);
    m_velocity = Vector2::ZERO;
    m_sprite->Disable();
    m_respawnText->m_text = (GameMode::GetCurrent()->m_respawnAllowed) ? RESPAWN_TEXT : DEAD_TEXT;
    m_respawnText->Enable();
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
    SetVortexShaderPosition(Vector2(-999.0f)); //If we don't reset this value somewhere, the player could get warped around holes that don't exist anymore in minigames.
    m_respawnText->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropPowerups()
{
    //No matter what, the chassis gets destroyed. Bye bye! ;D
    if (m_chassis)
    {
        delete m_chassis;
        m_chassis = nullptr;
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("DefaultChassis");
    }

    float powerUpPercentageDropped = 0.2f;
    int randomNumber = MathUtils::GetRandomIntFromZeroTo(4);
    switch (randomNumber)
    {
    case 0:
        if (m_activeEffect)
        {
            EjectActive();
            powerUpPercentageDropped /= 2.0f;
        }
        break;
    case 1:
        if (m_passiveEffect)
        {
            EjectPassive();
            powerUpPercentageDropped /= 2.0f;
        }
        break;
    case 2:
        if (m_weapon)
        {
            EjectWeapon();
            powerUpPercentageDropped /= 2.0f;
        }
        break;
    case 3:
        //Lose extra power ups this time.
        break;
    }
    

    unsigned int numPowerups = m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps();
    unsigned int numPowerupsToDrop = (unsigned int)(numPowerups * powerUpPercentageDropped);
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
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("DefaultChassis");
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
void PlayerShip::DebugUpdate(float deltaSeconds)
{
    if (InputSystem::instance->WasKeyJustPressed('1'))
    {
        m_powerupStatModifiers = Stats(-5.0f);
    }
    else if (InputSystem::instance->WasKeyJustPressed('2'))
    {
        m_powerupStatModifiers = Stats(0.0f);
    }
    else if (InputSystem::instance->WasKeyJustPressed('3'))
    {
        m_powerupStatModifiers = Stats(20.0f);
    }
    else if (InputSystem::instance->WasKeyJustPressed('4'))
    {
        m_powerupStatModifiers = Stats(30.0f);
    }

    static int paletteNumber = 0;
    if (InputSystem::instance->WasKeyJustPressed('I'))
    {
        ++paletteNumber;
        paletteNumber = (paletteNumber + (((PlayerPilot*)m_pilot)->m_playerNumber + 1)) % 16;
        float paletteIndex = static_cast<float>(paletteNumber) / 16.0f;
        m_sprite->m_material->SetFloatUniform("PaletteOffset", paletteIndex);
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
    float randomDegrees = MathUtils::GetRandomFloat(-80.0f, 80.0f);
    Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;

    if (pickedUpItem->IsPowerUp())
    {
        PowerUp* powerUp = ((PowerUp*)pickedUpItem);
        powerUp->ApplyPickupEffect(this);

        if (powerUp->m_powerUpType == PowerUpType::HP)
        {
            Heal(Stats::MIN_HP_VALUE);
        }

        GameMode::GetCurrent()->PlaySoundAt(powerUp->GetPickupSFXID(), GetPosition());
        ParticleSystem::PlayOneShotParticleEffect("PowerupPickup", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()), nullptr, powerUp->GetSpriteResource());

        TextSplash::CreateTextSplash(Stringf("%s Up", powerUp->GetPowerUpSpriteResourceName()), m_transform, velocity, RGBA::GBLIGHTGREEN);

        delete powerUp;
    }
    else if (pickedUpItem->IsWeapon())
    {
        EjectWeapon();
        m_weapon = (Weapon*)pickedUpItem;
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::GBLIGHTGREEN);
    }
    else if (pickedUpItem->IsChassis())
    {
        EjectChassis();
        m_chassis = (Chassis*)pickedUpItem;
        m_sprite->m_spriteResource = m_chassis->GetShipSpriteResource();
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::GBLIGHTGREEN);
    }
    else if (pickedUpItem->IsPassiveEffect())
    {
        if (m_passiveEffect)
        {
            EjectPassive();
        }
        m_passiveEffect = (PassiveEffect*)pickedUpItem;
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::GBLIGHTGREEN);

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
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::GBLIGHTGREEN);
    }
}

//-----------------------------------------------------------------------------------
bool PlayerShip::CanPickUp(Item* item)
{
    static const double FULL_TIME_SECONDS = 1.0f;
    static const double FULL_TIME_MILLISECONDS = FULL_TIME_SECONDS * 1000.0f;

    if (item->IsPowerUp())
    {
        PowerUp* powerUp = (PowerUp*)item;
        if (*m_powerupStatModifiers.GetStatReference(powerUp->m_powerUpType) < 20.0f)
        {
            return true;
        }
        else
        {
            double currTimeMilliseconds = GetCurrentTimeMilliseconds();
            if (currTimeMilliseconds - m_timeSinceFullDisplayedMilliseconds > FULL_TIME_MILLISECONDS)
            {
                m_timeSinceFullDisplayedMilliseconds = currTimeMilliseconds;

                float randomDegrees = MathUtils::GetRandomFloat(-80.0f, 80.0f);
                Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;
                TextSplash::CreateTextSplash("Full", m_transform, velocity, RGBA::RED);
            }

            return false;
        }
    }
    else if (item->IsWeapon() && m_weapon == nullptr)
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
    static const double EJECT_TIME_SECONDS = 0.5f;
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
