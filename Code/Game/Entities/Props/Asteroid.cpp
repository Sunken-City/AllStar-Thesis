#include "Game/Entities/Props/Asteroid.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Items/PowerUp.hpp"

const float Asteroid::MAX_ANGULAR_VELOCITY = 15.0f;
const float Asteroid::MIN_ASTEROID_SCALE = 0.5f;
const float Asteroid::MAX_ASTEROID_SCALE = 1.5f;

//-----------------------------------------------------------------------------------
Asteroid::Asteroid(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Asteroid", TheGame::GEOMETRY_LAYER);
    m_transform.SetScale(Vector2(MathUtils::GetRandomFloat(MIN_ASTEROID_SCALE, MAX_ASTEROID_SCALE)));
    m_sprite->m_transform.SetScale(m_transform.GetWorldScale());
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    //m_baseStats.hp += 3.0f;
    //Heal();
    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleBrown");

    m_isInvincible = true;
    
    //if (m_transform.GetWorldScale().x > MIN_ASTEROID_SCALE)
    {
        m_isImmobile = true;
    }
}

//-----------------------------------------------------------------------------------
Asteroid::~Asteroid()
{

}

//-----------------------------------------------------------------------------------
void Asteroid::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/cratePop.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), 1.0f);
    ParticleSystem::PlayOneShotParticleEffect("CrateDestroyed", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()));

//     GameMode* gamemode = GameMode::GetCurrent();
//     if (gamemode->m_isPlaying && m_transform.GetWorldScale().x > MIN_ASTEROID_SCALE / 2.0f)
//     {
//         Asteroid* asteroid1 = new Asteroid(m_transform.GetWorldPosition());
//         Asteroid* asteroid2 = new Asteroid(m_transform.GetWorldPosition());
//         asteroid1->m_transform.SetScale(m_transform.GetWorldScale() / 2.0f);
//         asteroid2->m_transform.SetScale(m_transform.GetWorldScale() / 2.0f);
//         asteroid1->ApplyImpulse(MathUtils::GetRandomDirectionVector() * 100.0f);
//         asteroid2->ApplyImpulse(MathUtils::GetRandomDirectionVector() * 100.0f);
//         if (MathUtils::CoinFlip())
//         {
//             gamemode->SpawnPickup(new PowerUp(), m_transform.GetWorldPosition());
//         }
// 
//         gamemode->SpawnEntityInGameWorld(asteroid1);
//         gamemode->SpawnEntityInGameWorld(asteroid2);
//     }
}

//-----------------------------------------------------------------------------------
void Asteroid::Update(float deltaSeconds)
{
    float newRotationDegrees = m_sprite->m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_sprite->m_transform.SetRotationDegrees(newRotationDegrees);
    Vector2 direction = Vector2::DegreesToDirection(-newRotationDegrees, Vector2::ZERO_DEGREES_UP);
}
