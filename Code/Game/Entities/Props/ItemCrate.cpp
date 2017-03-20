#include "Game/Entities/Props/ItemCrate.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Items/Item.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Items/DropTable.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const float ItemCrate::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
ItemCrate::ItemCrate(const Vector2& initialPosition) 
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("ItemBox", TheGame::CRATE_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(0.5f));
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleBeige");
    
    Heal();
    InitializeInventory(MathUtils::GetRandomIntFromZeroTo(MAX_NUM_PICKUPS_PER_BOX) + 1);
    GenerateItems();
    DecorateCrate();
}

//-----------------------------------------------------------------------------------
ItemCrate::~ItemCrate()
{
    delete m_itemHintSprite;
}

//-----------------------------------------------------------------------------------
void ItemCrate::Update(float deltaSeconds)
{
    float newRotation = m_transform.GetWorldRotationDegrees() + m_angularVelocity * deltaSeconds;
    Vector2 direction = Vector2::DegreesToDirection(-newRotation, Vector2::ZERO_DEGREES_UP);
    m_transform.SetRotationDegrees(newRotation);
}

//-----------------------------------------------------------------------------------
void ItemCrate::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/cratePop.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), 1.0f);
    ParticleSystem::PlayOneShotParticleEffect("CrateDestroyed", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()));
}

//-----------------------------------------------------------------------------------
void ItemCrate::GenerateItems()
{
    static const float ITEM_PERCENTAGE_CHANCE = 0.7f;
    static const float EQUIPMENT_PERCENTAGE_CHANCE = 1.0f - ITEM_PERCENTAGE_CHANCE;

    //Guarantee that we have a single power up so that we can make the crate have information on what's in it.
    m_inventory[0] = new PowerUp();

    unsigned int inventorySize = m_inventory.size();
    for (unsigned int i = 1; i < inventorySize; ++i)
    {
        if (MathUtils::GetRandomFloatFromZeroTo(1.0f) < ITEM_PERCENTAGE_CHANCE)
        {
            m_inventory[i] = new PowerUp();
        }
        else
        {
            switch (MathUtils::GetRandomIntFromZeroTo(4))
            {
            case 0:
                m_inventory[i] = GetRandomWeapon();
                break;
            case 1:
                m_inventory[i] = GetRandomChassis();
                break;
            case 2:
                m_inventory[i] = GetRandomActive();
                break;
            case 3:
                m_inventory[i] = GetRandomPassive();
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------------
void ItemCrate::DecorateCrate()
{
    PowerUp* powerUp = dynamic_cast<PowerUp*>(m_inventory[0]);
    ASSERT_OR_DIE(powerUp != nullptr, "First item in crate wasn't a power up, failed to decorate crate.");

    m_sprite->m_tintColor = PowerUp::GetPowerUpColor(powerUp->m_powerUpType);
    m_itemHintSprite = new Sprite(PowerUp::GetPowerUpSpriteResourceName(powerUp->m_powerUpType), TheGame::CRATE_DECORATION_LAYER);
    m_itemHintSprite->m_transform.SetParent(&m_transform);
}
