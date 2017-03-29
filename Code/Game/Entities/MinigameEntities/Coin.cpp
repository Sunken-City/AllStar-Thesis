#include "Game/Entities/MinigameEntities/Coin.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "../PlayerShip.hpp"
#include "Game/GameModes/Minigames/CoinGrabMinigameMode.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../TextSplash.hpp"

//-----------------------------------------------------------------------------------
Coin::Coin(const Vector2& position, int value /*= 0*/) 
    : Entity()
    , m_value(value)
{
    m_collidesWithBullets = false;
    m_noCollide = true;
    m_sprite = new Sprite("BronzeCoin", TheGame::POWER_UP_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);

    if (value == 0)
    {
        if (MathUtils::CoinFlip() && MathUtils::CoinFlip())
        {
            m_value = SILVER_VALUE;

            if (MathUtils::CoinFlip() && MathUtils::CoinFlip())
            {
                m_value = GOLD_VALUE;
            }
        }
        else
        {
            m_value = BRONZE_VALUE;
        }
    }
    else
    {
        m_value = value;
    }

    switch (m_value)
    {
    case SILVER_VALUE:
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("SilverCoin");
        break;
    case GOLD_VALUE:
        m_sprite->m_spriteResource = ResourceDatabase::instance->GetSpriteResource("GoldCoin");
        break;
    }
    
    CalculateCollisionRadius();

    float x = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    float y = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    SetPosition(position + Vector2(x, y));
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(15.0f));
    m_baseStats.hp = 10.0f;
    m_currentHp = m_baseStats.hp;

    float directionDegrees = MathUtils::GetRandomFloatFromZeroTo(360.0f);
    m_velocity = Vector2::DegreesToDirection(directionDegrees) * 10.0f;
}

//-----------------------------------------------------------------------------------
Coin::~Coin()
{
}

//-----------------------------------------------------------------------------------
void Coin::Update(float deltaSeconds)
{
    const float FLASH_AGE_SECONDS = m_maxAge - 5.0f;
    Entity::Update(deltaSeconds);

    Vector2 newScale = Vector2(1.0f) + Vector2(sin(m_age * 2.0f) / 4.0f);
    m_transform.SetScale(newScale);
    m_velocity *= 0.9f;
    Vector2 attemptedPosition = GetPosition() + (m_velocity * deltaSeconds);
    SetPosition(attemptedPosition);
    
    if (m_age > FLASH_AGE_SECONDS)
    {
        float alphaValue = static_cast<int>(m_age * 10.0f) % 2 == 0 ? 1.0f : 0.0f;
        m_sprite->m_tintColor.SetAlphaFloat(alphaValue);
    }
    if (m_age > m_maxAge)
    {
        m_isDead = true;
    }
}

//-----------------------------------------------------------------------------------
void Coin::Render() const
{

}

//-----------------------------------------------------------------------------------
void Coin::ResolveCollision(Entity* otherEntity)
{
    if (m_age < 0.05f)
    {
        return;
    }

    //Only push away against other Coins.
    if (otherEntity->IsPickup() || (otherEntity->IsProp() && !otherEntity->m_isInvincible))
    {
        Vector2 myPosition = GetPosition();
        Vector2 otherPosition = otherEntity->GetPosition();
        Vector2 displacementFromOtherToMe = myPosition - otherPosition;
        Vector2 directionFromOtherToMe = displacementFromOtherToMe.GetNorm();

        float distanceBetweenEntities = displacementFromOtherToMe.CalculateMagnitude();
        float sumOfRadii = m_collisionRadius + otherEntity->m_collisionRadius;
        float overlapDistance = sumOfRadii - distanceBetweenEntities;
        float pushDistance = overlapDistance * 0.5f;
        Vector2 myPositionCorrection = directionFromOtherToMe * pushDistance;
        if (!m_isImmobile)
        {
            SetPosition(myPosition + myPositionCorrection);
        }
        if (!otherEntity->m_isImmobile)
        {
            otherEntity->SetPosition(otherPosition - myPositionCorrection);
        }
    }

    for (PlayerShip* player : TheGame::instance->m_players)
    {
        if ((Entity*)player == otherEntity && !player->m_isDead)
        {
            CoinGrabMinigameMode* mode = dynamic_cast<CoinGrabMinigameMode*>(GameMode::GetCurrent());
            ASSERT_OR_DIE(mode, "Picked up a coin, but we aren't in CoinGrabMinigameMode");

            this->m_isDead = true;
            mode->RecordPlayerPickupCoin(player, m_value);
            mode->PlaySoundAt(AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/coin.wav"), GetPosition());
            ParticleSystem::PlayOneShotParticleEffect("PowerupPickup", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()), nullptr, m_sprite->m_spriteResource);

            TextSplash::CreateTextSplash(Stringf("+%i", m_value), m_transform, Vector2(0.0f, 1.0f), RGBA::YELLOW);
        }
    }
}
