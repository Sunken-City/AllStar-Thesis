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
#include "../Items/Chassis/AttractorChassis.hpp"
#include "../Items/Passives/SpecialTrailPassive.hpp"
#include "../Items/Actives/ShieldActive.hpp"
#include "../Items/Actives/BoostActive.hpp"
#include "../Items/Chassis/TankChassis.hpp"
#include "../Items/Weapons/SpreadShot.hpp"
#include "../Items/Passives/SprayAndPrayPassive.hpp"
#include "../Items/Weapons/WaveGun.hpp"
#include <gl/GL.h>
#include "Engine/Renderer/2D/BarGraphRenderable2D.hpp"
#include "Engine/Fonts/BitmapFont.hpp"
#include "Engine/Core/RunInSeconds.hpp"
#include "../GameStrings.hpp"
#include "../Items/Actives/TeleportActive.hpp"
#include "../Items/Passives/SharpshooterPassive.hpp"
#include "../Items/Actives/ReflectorActive.hpp"

const Vector2 PlayerShip::DEFAULT_SCALE = Vector2(2.0f);
const char* PlayerShip::RESPAWN_TEXT = "Press Start to Respawn";
const char* PlayerShip::DEAD_TEXT = "You have Died";

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip(PlayerPilot* pilot)
    : Ship((Pilot*)pilot)
    , m_respawnText(new TextRenderable2D(RESPAWN_TEXT, Transform2D(Vector2(0.0f, 0.0f)), TheGame::FBO_FREE_TEXT_LAYER, false))
    , m_healthText(new TextRenderable2D("HP:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_shieldText(new TextRenderable2D("SH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_tpText(new TextRenderable2D("MPH:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_scoreText(new TextRenderable2D("DPS:@@@", Transform2D(Vector2(0.0f, 0.0f)), TheGame::TEXT_LAYER))
    , m_healthBar(new BarGraphRenderable2D(AABB2(Vector2(-0.05f, 0.0f), Vector2(5.55f, 0.6f)), RGBA::RED, RGBA::GRAY, TheGame::BACKGROUND_UI_LAYER))
    , m_teleportBar(new BarGraphRenderable2D(AABB2(Vector2(0.05f, 1.8f), Vector2(-4.791f, 1.4f)), RGBA::PURPLE, RGBA::GRAY, TheGame::BACKGROUND_UI_LAYER))
    , m_shieldBar(new BarGraphRenderable2D(AABB2(Vector2(-0.05f, 0.6f), Vector2(4.1f, 1.2f)), RGBA::CERULEAN, RGBA::GRAY, TheGame::BACKGROUND_UI_LAYER))
    , m_paletteSwapShader(new ShaderProgram("Data/Shaders/default2D.vert", "Data/Shaders/paletteSwap2D.frag"))
    , m_cooldownShader(new ShaderProgram("Data/Shaders/noWarp2D.vert", "Data/Shaders/cooldown.frag"))
{
    m_paletteSwapShader->BindUniformBuffer("vortexInfo", TheGame::instance->m_bindingPoint);
    m_isDead = false;
    m_paletteSwapMaterial = new Material(m_paletteSwapShader, SpriteGameRenderer::instance->m_defaultRenderState);
    m_paletteSwapMaterial->ReplaceSampler(Renderer::instance->CreateSampler(GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP));
    m_cooldownMaterial = new Material(m_cooldownShader, SpriteGameRenderer::instance->m_defaultRenderState);
    m_playerTintedUIMaterial = new Material(new ShaderProgram("Data/Shaders/noWarp2D.vert", "Data/Shaders/paletteSwap2D.frag"), SpriteGameRenderer::instance->m_defaultRenderState);

    m_sprite = new Sprite("DefaultChassis", TheGame::PLAYER_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    m_sprite->m_material = m_paletteSwapMaterial;
    float paletteIndex = ((float)((PlayerPilot*)m_pilot)->m_playerNumber + 1.0f) / 16.0f;
    m_sprite->m_material->SetFloatUniform(paletteOffsetUniform, paletteIndex);
    m_playerTintedUIMaterial->SetFloatUniform(paletteOffsetUniform, paletteIndex);
    //m_sprite->m_recolorMode = (SpriteRecolorMode)(((PlayerPilot*)m_pilot)->m_playerNumber + 4);
    m_sprite->m_material->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ShipColorPalettes")->m_texture);
    m_playerTintedUIMaterial->SetEmissiveTexture(ResourceDatabase::instance->GetSpriteResource("ShipColorPalettes")->m_texture);
    m_transform.SetScale(DEFAULT_SCALE);
    m_transform.SetPosition(Vector2(1000.0f));

    m_shieldSprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("RecolorableShield");
    m_shieldSprite->m_material = m_sprite->m_material;
    m_shipTrail->m_emitters[0]->m_materialOverride = m_sprite->m_material;
    m_shipTrail->m_emitters[0]->m_spriteOverride = ResourceDatabase::instance->GetSpriteResource("RecolorableBeamTrail");
    //m_shieldSprite->m_tintColor = GetPlayerColor();
    //m_shipTrail->m_colorOverride = GetPlayerColor();
    m_factionColor = GetPlayerColor();
    InitializeUI();
    InitializeStatGraph();

    CalculateCollisionRadius();
    m_currentHp = CalculateHpValue();

    if (g_nearlyInvulnerable)
    {
        m_currentHp = 99999999.0f;
    }
    if (g_spawnWithDebugLoadout)
    {
        //PickUpItem(new MissileLauncher());
        //PickUpItem(new TankChassis());
        //PickUpItem(new ReflectorActive());
        //PickUpItem(new CloakPassive());
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
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_teleportBar->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomRight(&m_tpText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_playerData->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_healthText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_healthBar->m_transform);
    SpriteGameRenderer::instance->RemoveAnchorBottomLeft(&m_shieldBar->m_transform);
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
    delete m_healthBar;
    delete m_teleportBar;
    delete m_shieldBar;
    delete m_tpText;
    delete m_scoreText;
    delete m_shieldDownEffect->m_shaderProgram;
    delete m_shieldDownEffect;
    delete m_playerTintedUIMaterial->m_shaderProgram;
    delete m_playerTintedUIMaterial;

    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_shieldDownEffect, TheGame::FULL_SCREEN_EFFECT_LAYER);

    delete m_statValuesBG;
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        delete m_statValues[i];
        delete m_statSprites[i];
        delete m_statBarGraphs[i];
    }

    delete m_paletteSwapShader;
    delete m_paletteSwapMaterial;

    delete m_cooldownShader;
    delete m_cooldownMaterial;
}

//-----------------------------------------------------------------------------------
void PlayerShip::InitializeUI()
{
    m_equipUI = new Sprite("EquipmentUI", TheGame::UI_LAYER);
    //m_equipUI->m_tintColor = GetPlayerColor();
    m_equipUI->m_tintColor.SetAlphaFloat(0.75f);
    m_equipUI->m_transform.SetScale(Vector2(1.0f));
    m_equipUI->m_transform.SetPosition(Vector2(-5.6f, 0.0f));
    m_equipUI->m_material = m_playerTintedUIMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_equipUI->m_transform);

    m_playerData = new Sprite("HealthUI", TheGame::UI_LAYER);
    //m_playerData->m_tintColor = GetPlayerColor();
    m_playerData->m_transform.SetScale(Vector2(1.0f));
    m_playerData->m_transform.SetPosition(Vector2::ZERO);
    m_playerData->m_material = m_playerTintedUIMaterial;
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_playerData->m_transform);
    m_healthBar->m_fillColor.SetAlphaFloat(0.75f);
    m_teleportBar->m_fillColor.SetAlphaFloat(0.75f);
    m_shieldBar->m_fillColor.SetAlphaFloat(0.75f);

    m_currentWeaponUI = new Sprite("EmptyWeaponSlot", TheGame::BACKGROUND_UI_LAYER);
    m_currentWeaponUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentWeaponUI->m_transform.SetScale(Vector2(2.1f));
    m_currentWeaponUI->m_transform.SetPosition(Vector2(-3.46f, 0.725f));
    m_currentWeaponUI->m_material = TheGame::instance->m_UIMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentWeaponUI->m_transform);

    m_currentActiveUI = new Sprite("EmptyActiveSlot", TheGame::BACKGROUND_UI_LAYER);
    m_currentActiveUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentActiveUI->m_transform.SetScale(Vector2(2.4f));
    m_currentActiveUI->m_transform.SetPosition(Vector2(-4.777f, 0.725f));
    m_currentActiveUI->m_material = m_cooldownMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentActiveUI->m_transform);

    m_currentChassisUI = new Sprite("EmptyChassisSlot", TheGame::BACKGROUND_UI_LAYER);
    m_currentChassisUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentChassisUI->m_transform.SetScale(Vector2(2.1f));
    m_currentChassisUI->m_transform.SetPosition(Vector2(-1.0f, 0.725f));
    m_currentChassisUI->m_material = TheGame::instance->m_UIMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentChassisUI->m_transform);

    m_currentPassiveUI = new Sprite("EmptyPassiveSlot", TheGame::BACKGROUND_UI_LAYER);
    m_currentPassiveUI->m_tintColor.SetAlphaFloat(0.75f);
    m_currentPassiveUI->m_transform.SetScale(Vector2(2.1f));
    m_currentPassiveUI->m_transform.SetPosition(Vector2(-2.23f, 0.725f));
    m_currentPassiveUI->m_material = TheGame::instance->m_UIMaterial;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_currentPassiveUI->m_transform);
          
    m_respawnText->m_color = RGBA::WHITE;
    m_healthText->m_color = RGBA::WHITE;
    m_healthText->m_color.SetAlphaFloat(0.75f);
    m_shieldText->m_color = RGBA::WHITE;
    m_shieldText->m_color.SetAlphaFloat(0.75f);
    m_tpText->m_color = RGBA::WHITE;
    m_scoreText->m_color = RGBA::GBLIGHTGREEN;
    m_healthText->m_transform.SetPosition(Vector2(1.1f, 0.3f));
    m_shieldText->m_transform.SetPosition(Vector2(1.1f, 0.9f));
    m_tpText->m_transform.SetPosition(Vector2(-1.0f, 1.6f));
    m_scoreText->m_transform.SetPosition(Vector2(2.0f, 1.8f));
    m_respawnText->m_transform.SetScale(Vector2(1.0f));
    m_healthText->m_transform.SetScale(Vector2(1.0f));
    m_shieldText->m_transform.SetScale(Vector2(1.0f));
    m_tpText->m_transform.SetScale(Vector2(1.0f));
    m_scoreText->m_transform.SetScale(Vector2(1.0f));
    m_respawnText->m_fontSize = 0.4f;
    m_healthText->m_fontSize = 0.25f;
    m_shieldText->m_fontSize = 0.25f;
    m_tpText->m_fontSize = 0.25f;
    m_scoreText->m_fontSize = 0.4f;
    SpriteGameRenderer::instance->AnchorBottomRight(&m_teleportBar->m_transform);
    SpriteGameRenderer::instance->AnchorBottomRight(&m_tpText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_healthText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_shieldText->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_healthBar->m_transform);
    SpriteGameRenderer::instance->AnchorBottomLeft(&m_shieldBar->m_transform);
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
    m_healthBar->m_viewableBy = visibilityFilter;
    m_teleportBar->m_viewableBy = visibilityFilter;
    m_shieldBar->m_viewableBy = visibilityFilter;
    m_tpText->m_viewableBy = visibilityFilter;
    m_scoreText->m_viewableBy = visibilityFilter;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    if (m_isDead)
    {
        m_shieldDownEffect->SetFloatUniform(gEffectTimeUniform, (float)GetCurrentTimeSeconds());
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
    if (!m_abilitiesLocked)
    {
        UpdateEquips(deltaSeconds);
    }
    UpdatePlayerUI(deltaSeconds);
    DebugUpdate(deltaSeconds);
}

//-----------------------------------------------------------------------------------
void PlayerShip::UpdatePlayerUI(float deltaSeconds)
{
    static const size_t gPercentageUniform = std::hash<std::string>{}("gPercentage");
    static const size_t gPercentagePerUseUniform = std::hash<std::string>{}("gPercentagePerUse");
    UNUSED(deltaSeconds);
    static const float MIN_UI_ALPHA = 0.1f;
    static const float MAX_UI_ALPHA = 1.0f;
    static const float MAX_BAR_UI_ALPHA = 0.75f;

    m_currentWeaponUI->m_spriteResource = m_weapon ? m_weapon->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyWeaponSlot");
    m_currentActiveUI->m_spriteResource = m_activeEffect ? m_activeEffect->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyActiveSlot");
    m_currentChassisUI->m_spriteResource = m_chassis ? m_chassis->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyChassisSlot");
    m_currentPassiveUI->m_spriteResource = m_passiveEffect ? m_passiveEffect->GetSpriteResource() : ResourceDatabase::instance->GetSpriteResource("EmptyPassiveSlot");

    m_healthText->m_text = Stringf("HP: %03i/%03i", static_cast<int>(ceil(m_currentHp)), static_cast<int>(ceil(CalculateHpValue())));
    m_shieldText->m_text = Stringf("SH: %03i/%03i", static_cast<int>(ceil(m_currentShieldHealth)), static_cast<int>(ceil(CalculateShieldCapacityValue())));
    m_tpText->m_text = Stringf("TP: %2.2f%s", m_warpFreebieActive.m_energy * 100.0f, "%");
    m_tpText->m_color = m_warpFreebieActive.m_energy > m_warpFreebieActive.m_costToActivate ? RGBA::WHITE : RGBA::RED;
    m_scoreText->m_text = Stringf("LVL: %03i", m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps());

    if (m_activeEffect)
    {
        m_cooldownMaterial->SetFloatUniform(gPercentageUniform, m_activeEffect->m_energy);
        m_cooldownMaterial->SetFloatUniform(gPercentagePerUseUniform, m_activeEffect->m_costToActivate);
    }
    else
    {
        m_cooldownMaterial->SetFloatUniform(gPercentageUniform, 1.0f);
    }
        
    float lerpAmount = Clamp01(fabs(GameMode::GetCurrent()->GetArenaBounds().mins.y - m_transform.GetWorldPosition().y) * 0.25f);
    m_currentWeaponUI->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_currentActiveUI->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_currentChassisUI->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_currentPassiveUI->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_equipUI->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_playerData->m_tintColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_healthText->m_color.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_shieldText->m_color.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_tpText->m_color.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_scoreText->m_color.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_UI_ALPHA));
    m_healthBar->m_fillColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));
    m_teleportBar->m_fillColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));
    m_shieldBar->m_fillColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));
    m_healthBar->m_unfilledColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));
    m_teleportBar->m_unfilledColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));
    m_shieldBar->m_unfilledColor.SetAlphaFloat(Lerp<float>(lerpAmount, MIN_UI_ALPHA, MAX_BAR_UI_ALPHA));

    GameMode* current = GameMode::GetCurrent();
    if (current && current->m_isPlaying && m_warpFreebieActive.m_energy > m_warpFreebieActive.m_costToActivate && m_tpChargeLastFrame < m_warpFreebieActive.m_costToActivate)
    {
        ParticleSystem* system = ParticleSystem::PlayOneShotParticleEffect("EyeTwinkle", TheGame::UI_LAYER, m_tpText->m_transform);
        system->m_emitters[0]->m_materialOverride = TheGame::instance->m_UIMaterial;
        system->m_emitters[0]->m_spriteOverride = ResourceDatabase::instance->GetSpriteResource("White4Star");
        system->m_viewableBy = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);
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
    if (m_pilot->m_inputMap.WasJustPressed("Warp") && IsAlive())
    {
        NamedProperties props;
        props.Set<Ship*>("ShipPtr", (Ship*)this);
        m_warpFreebieActive.Activate(props);
    }
    m_tpChargeLastFrame = m_warpFreebieActive.m_energy;
    m_warpFreebieActive.Update(deltaSeconds);
    m_teleportBar->SetPercentageFilled(m_warpFreebieActive.m_energy);
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
    m_tpText->Disable();
    m_scoreText->Disable();
    m_healthBar->Disable();
    m_teleportBar->Disable();
    m_shieldBar->Disable();
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
    m_tpText->Enable();
    m_scoreText->Enable();
    m_healthBar->Enable();
    m_teleportBar->Enable();
    m_shieldBar->Enable();
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
            m_shieldDownEffect->SetFloatUniform(gEffectTimeUniform, (float)GetCurrentTimeSeconds());
        }
        ratioOfDamage *= 4.0f;
        ratioOfDamage = Clamp(ratioOfDamage, 0.0f, 1.0f);
        m_pilot->LightRumble(ratioOfDamage, 0.25f);
        m_healthBar->SetPercentageFilled(m_currentHp / CalculateHpValue());
    }
    m_pilot->HeavyRumble(ratioOfDamage, 0.25f);

    return returnValue;
}

//-----------------------------------------------------------------------------------
void PlayerShip::Heal(float healValue /*= 99999999.0f*/)
{
    Ship::Heal(healValue);
    m_healthBar->SetPercentageFilled(m_currentHp / CalculateHpValue());
}

//-----------------------------------------------------------------------------------
void PlayerShip::SetShieldHealth(float newShieldValue /*= 99999999.0f*/)
{
    Ship::SetShieldHealth(newShieldValue);
    m_shieldBar->SetPercentageFilled(m_currentShieldHealth / CalculateShieldCapacityValue());
}

//-----------------------------------------------------------------------------------
void PlayerShip::Die()
{
    Ship::Die();
    if (GameMode::GetCurrent()->m_dropItemsOnDeath)
    {
        DropPowerupsAndEquipment();
    }

    GameMode::GetCurrent()->RecordPlayerDeath(this);
    SpriteGameRenderer::instance->AddScreenshakeMagnitude(0.4f, Vector2::ZERO, ((PlayerPilot*)m_pilot)->m_playerNumber);
    m_velocity = Vector2::ZERO;
    m_sprite->Disable();
    m_respawnText->m_text = (GameMode::GetCurrent()->m_respawnAllowed) ? RESPAWN_TEXT : DEAD_TEXT;
    m_respawnText->Enable();
    m_healthBar->SetPercentageFilled(0.0f);
    m_shieldBar->SetPercentageFilled(0.0f);
}

//-----------------------------------------------------------------------------------
void PlayerShip::Respawn()
{
    m_isDead = false;
    Heal();
    SetShieldHealth();
    m_velocity = Vector2::ZERO;
    m_shipTrail->Flush();
    m_sprite->Enable();
    if (TheGame::instance->m_currentGameMode)
    {
        SetPosition(TheGame::instance->m_currentGameMode->GetPlayerSpawnPoint(((PlayerPilot*)m_pilot)->m_playerNumber));
    }
    m_respawnText->Disable();
    m_healthBar->SetPercentageFilled(1.0f);
    m_teleportBar->SetPercentageFilled(1.0f);
    m_warpFreebieActive.m_energy = 1.0f;
    m_shieldBar->SetPercentageFilled(1.0f);
    if (m_activeEffect)
    {
        m_activeEffect->m_energy = 1.0f;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropPowerupsAndEquipment()
{
    static const unsigned int AVERAGE_NUM_PICKUPS = 84;
    static const unsigned int MIN_NUM_PICKUPS_DROPPED = 3;
    static const float ABSOLUTE_MAX_NUM_PICKUPS = 240.0f;
    static const float MAX_PERCENTAGE_LOST = 0.3f;
    unsigned int numPowerups = m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps();
    float powerUpPercentageDropped = MathUtils::SmoothStart2((float)numPowerups / ABSOLUTE_MAX_NUM_PICKUPS) * MAX_PERCENTAGE_LOST;
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

    //No matter what, the chassis gets destroyed. Bye bye! ;D
    if (m_chassis)
    {
        m_chassis->Deactivate(NamedProperties::NONE);
        delete m_chassis;
        m_chassis = nullptr;
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("DefaultChassis");
    }

    unsigned int numPowerupsToDrop = (unsigned int)ceil((float)numPowerups * powerUpPercentageDropped);
    numPowerupsToDrop = (numPowerupsToDrop < MIN_NUM_PICKUPS_DROPPED) ? MIN_NUM_PICKUPS_DROPPED : numPowerupsToDrop; //Ensure we drop more than the minimum
    numPowerupsToDrop = (numPowerups < numPowerupsToDrop) ? numPowerups : numPowerupsToDrop; //...Unless we're too poor.

    for (unsigned int i = 0; i < numPowerupsToDrop; ++i)
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
        m_chassis->Deactivate(NamedProperties::NONE);
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
    UNUSED(deltaSeconds);
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
}

//-----------------------------------------------------------------------------------
void PlayerShip::InitializeStatGraph()
{
    float SCALE_FACTOR = 1.0f;
    if (TheGame::instance->m_numberOfPlayers == 2)
    {
        SCALE_FACTOR = 0.75f;
    }
    else if (TheGame::instance->m_numberOfPlayers == 3)
    {
        SCALE_FACTOR = 0.65f;
    }
    
    const float SPACE_PER_ROW = 0.6f;
    const float HALF_SPACE_PER_ROW = SPACE_PER_ROW * 0.5f;
    const float BAR_GRAPH_START = 1.0f * SCALE_FACTOR;
    const float BAR_GRAPH_LENGTH = 7.0f * SCALE_FACTOR;
    const float STARTING_Y_VALUE = 2.5f * SCALE_FACTOR;
    const float STARTING_X_VALUE = -4.0f * SCALE_FACTOR;
    const float SPRITE_X_VALUE = -0.8f * SCALE_FACTOR;

    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);

    m_statValuesBG = new Sprite("Quad", TheGame::STAT_GRAPH_LAYER_BACKGROUND, true);
    m_statValuesBG->m_transform.SetScale(Vector2(100.0f, 100.0f));
    m_statValuesBG->m_tintColor = RGBA::GBDARKGREEN;
    m_statValuesBG->m_tintColor.SetAlphaFloat(0.75f);
    m_statValuesBG->m_transform.SetPosition(Vector2::ZERO);
    m_statValuesBG->m_viewableBy = visibilityFilter;
    m_statValuesBG->Disable();

    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        PowerUpType type = (PowerUpType)i;
        TextRenderable2D* statLine = new TextRenderable2D("This is a bug", Transform2D(Vector2(STARTING_X_VALUE, STARTING_Y_VALUE - (SPACE_PER_ROW * i))), TheGame::STAT_GRAPH_LAYER_TEXT);
        statLine->m_fontSize = 0.5f * SCALE_FACTOR;
        statLine->m_font = BitmapFont::CreateOrGetFont("FixedSys");
        statLine->m_color = PowerUp::GetPowerUpColor(type);
        statLine->Disable();
        statLine->m_viewableBy = visibilityFilter;
        m_statValues[i] = statLine;

        Sprite* statSprite = new Sprite(PowerUp::GetPowerUpSpriteResourceName(type), TheGame::STAT_GRAPH_LAYER_TEXT);
        statSprite->m_transform.SetPosition(Vector2(SPRITE_X_VALUE, STARTING_Y_VALUE - (SPACE_PER_ROW * i)));
        statSprite->m_transform.SetScale(Vector2(0.15f));
        statSprite->Disable();
        statSprite->m_viewableBy = visibilityFilter;
        statSprite->m_material = TheGame::instance->m_UIMaterial;
        m_statSprites[i] = statSprite;

        BarGraphRenderable2D* statGraph = new BarGraphRenderable2D(
            AABB2(
                Vector2(BAR_GRAPH_START, (STARTING_Y_VALUE - HALF_SPACE_PER_ROW) - (SPACE_PER_ROW * i)), 
                Vector2(BAR_GRAPH_START + BAR_GRAPH_LENGTH, (STARTING_Y_VALUE + HALF_SPACE_PER_ROW) - (SPACE_PER_ROW * i))
                ), 
            PowerUp::GetPowerUpColor(type), 
            RGBA::CLEAR, TheGame::STAT_GRAPH_LAYER);

        statGraph->SetPercentageFilled(0.0f);
        statGraph->Disable();
        statGraph->m_viewableBy = visibilityFilter;
        m_statBarGraphs[i] = statGraph;
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::ShowStatGraph()
{
    m_statValuesBG->Enable();
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        PowerUpType type = (PowerUpType)i;
        m_statValues[i]->m_text = Stringf("%-20sx%2i", PowerUp::GetPowerUpSpriteResourceName(type), static_cast<int>(*m_powerupStatModifiers.GetStatReference(type)));
        m_statValues[i]->Enable();
        m_statSprites[i]->Enable();
        m_statBarGraphs[i]->Enable();
        m_statBarGraphs[i]->SetPercentageFilled((*m_powerupStatModifiers.GetStatReference(type)) / 20.0f);
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::SlowShowStatGraph()
{
    const float TIME_SECONDS_PER_BAR = 0.25f;
    m_statValuesBG->Enable();
    uchar visibilityFilter = (uchar)SpriteGameRenderer::GetVisibilityFilterForPlayerNumber(static_cast<PlayerPilot*>(m_pilot)->m_playerNumber);

    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        PowerUpType type = (PowerUpType)i;
        m_statValues[i]->m_text = Stringf("%-20sx%2i", PowerUp::GetPowerUpSpriteResourceName(type), static_cast<int>(*m_powerupStatModifiers.GetStatReference(type)));
        m_statValues[i]->Enable();
        m_statSprites[i]->Enable();
        m_statBarGraphs[i]->Enable();
        m_statBarGraphs[i]->SetPercentageFilled(0.0f);
        RunAfterSeconds([=]()
        {
            m_statBarGraphs[i]->SetPercentageFilled((*m_powerupStatModifiers.GetStatReference(type)) / 20.0f);
        }, TIME_SECONDS_PER_BAR * i);

        if ((int)(*m_powerupStatModifiers.GetStatReference(type)) >= 17)
        {
            RunAfterSeconds([=]()
            {
               TextSplash* textSplash = TextSplash::CreateTextSplash(GameStrings::GetAwesomeStatString(), m_statBarGraphs[i]->m_filledMaxsTransform, Vector2::ONE, RGBA::YELLOW, TheGame::STAT_GRAPH_LAYER_TEXT);
               textSplash->m_textRenderable->m_viewableBy = visibilityFilter;
            }, TIME_SECONDS_PER_BAR * (i + 2));
        }
        else if ((int)(*m_powerupStatModifiers.GetStatReference(type)) <= 3)
        {
            RunAfterSeconds([=]()
            {
                TextSplash* textSplash = TextSplash::CreateTextSplash(GameStrings::GetTerribleStatString(), m_statBarGraphs[i]->m_filledMaxsTransform, Vector2::ONE, RGBA::YELLOW, TheGame::STAT_GRAPH_LAYER_TEXT);
                textSplash->m_textRenderable->m_viewableBy = visibilityFilter;
            }, TIME_SECONDS_PER_BAR * (i + 2));
        }
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::HideStatGraph()
{
    m_statValuesBG->Disable();
    for (unsigned int i = 0; i < (unsigned int)PowerUpType::NUM_POWERUP_TYPES; ++i)
    {
        m_statValues[i]->Disable();
        m_statSprites[i]->Disable();
        m_statBarGraphs[i]->SetPercentageFilled(0.0f);
        m_statBarGraphs[i]->Disable();
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
        const float PREVIOUS_HP = CalculateHpValue();
        PowerUp* powerUp = ((PowerUp*)pickedUpItem);
        powerUp->ApplyPickupEffect(this);

        if (powerUp->m_powerUpType == PowerUpType::HP)
        {
            Heal(CalculateHpValue() - PREVIOUS_HP);
        }

        GameMode::GetCurrent()->PlaySoundAt(powerUp->GetPickupSFXID(), GetPosition());
        ParticleSystem::PlayOneShotParticleEffect("PowerupPickup", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()), nullptr, powerUp->GetSpriteResource());

        TextSplash::CreateTextSplash(Stringf("+ %s", powerUp->GetPowerUpSpriteResourceName()), m_transform, velocity, PowerUp::GetPowerUpColor(powerUp->m_powerUpType));

        delete powerUp;
    }
    else if (pickedUpItem->IsWeapon())
    {
        EjectWeapon();
        m_weapon = (Weapon*)pickedUpItem;
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::RED);
    }
    else if (pickedUpItem->IsChassis())
    {
        EjectChassis();
        m_chassis = (Chassis*)pickedUpItem;
        m_sprite->m_spriteResource = m_chassis->GetShipSpriteResource();
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::YELLOW);
        NamedProperties props;
        props.Set<Ship*>("ShipPtr", (Ship*)this);
        m_chassis->Activate(props);
    }
    else if (pickedUpItem->IsPassiveEffect())
    {
        if (m_passiveEffect)
        {
            EjectPassive();
        }
        m_passiveEffect = (PassiveEffect*)pickedUpItem;
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::CERULEAN);

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
        TextSplash::CreateTextSplash(Stringf("%s", pickedUpItem->m_name), m_transform, velocity, RGBA::GREEN);
    }
}

//-----------------------------------------------------------------------------------
bool PlayerShip::CanPickUp(Item* item)
{
    double currentTimeMilliseconds = GetCurrentTimeMilliseconds();
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
            if (currTimeMilliseconds - m_timeSinceFullDisplayedMilliseconds > FULL_MESSAGE_TIME_MILLISECONDS)
            {
                m_timeSinceFullDisplayedMilliseconds = currTimeMilliseconds;

                float randomDegrees = MathUtils::GetRandomFloat(-80.0f, 80.0f);
                Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;
                TextSplash::CreateTextSplash("Full", m_transform, velocity, RGBA::RED);
            }

            return false;
        }
    }
    else if (item->IsWeapon() && (currentTimeMilliseconds - m_weaponBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS) && (m_pilot->m_inputMap.IsDown("EjectWeapon")))
    {
        EjectWeapon();
        m_weaponBeginEjectMilliseconds = currentTimeMilliseconds;
        return true;
    }
    else if (item->IsChassis() && (currentTimeMilliseconds - m_chassisBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS) && (m_pilot->m_inputMap.IsDown("EjectChassis")))
    {
        EjectChassis();
        m_chassisBeginEjectMilliseconds = currentTimeMilliseconds;
        return true;
    }
    else if (item->IsPassiveEffect() && (currentTimeMilliseconds - m_passiveBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS) && (m_pilot->m_inputMap.IsDown("EjectPassive")))
    {
        EjectPassive();
        m_passiveBeginEjectMilliseconds = currentTimeMilliseconds;
        return true;
    }
    else if (item->IsActiveEffect() && (currentTimeMilliseconds - m_activeBeginEjectMilliseconds > EJECT_TIME_MILLISECONDS) && (m_pilot->m_inputMap.IsDown("EjectActive")))
    {
        EjectActive();
        m_activeBeginEjectMilliseconds = currentTimeMilliseconds;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------
void PlayerShip::CheckToEjectEquipment(float)
{
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
}

//-----------------------------------------------------------------------------------
void PlayerShip::SetPaletteOffset(int paletteIndex)
{
    float paletteUV = static_cast<float>(paletteIndex) / 16.0f;
    m_sprite->m_material->SetFloatUniform(paletteOffsetUniform, paletteUV);
    m_playerTintedUIMaterial->SetFloatUniform(paletteOffsetUniform, paletteUV);
}
