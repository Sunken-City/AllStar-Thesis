#include "Game/Entities/TextSplash.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "Game/TheGame.hpp"

std::vector<TextSplash*, UntrackedAllocator<TextSplash*>> TextSplash::m_textSplashes;

//-----------------------------------------------------------------------------------
TextSplash::TextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color)
    : m_velocity(velocity)
{
    m_textRenderable = new TextRenderable2D(text, spawnTransform, TheGame::TEXT_PARTICLE_LAYER, true);
    m_textRenderable->m_fontSize /= 4.0f;
    m_textRenderable->m_transform.SetRotationDegrees(0.0f);
    m_textRenderable->m_color = color;
}

//-----------------------------------------------------------------------------------
TextSplash::~TextSplash()
{
    delete m_textRenderable;
    m_textRenderable = nullptr;
}

//-----------------------------------------------------------------------------------
void TextSplash::Cleanup()
{
    for (TextSplash* text : m_textSplashes)
    {
        delete text;
    }
    m_textSplashes.clear();
}

//-----------------------------------------------------------------------------------
void TextSplash::CreateTextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color)
{
    m_textSplashes.push_back(new TextSplash(text, spawnTransform, velocity, color));
}

//-----------------------------------------------------------------------------------
void TextSplash::Update(float deltaSeconds)
{ 
    for (TextSplash* text : m_textSplashes)
    {
        text->m_age += deltaSeconds;
        Vector2 newPosition = text->m_textRenderable->m_transform.GetWorldPosition() + (text->m_velocity * deltaSeconds);
        text->m_textRenderable->m_transform.SetPosition(newPosition);
    }

    for (auto iter = m_textSplashes.begin(); iter != m_textSplashes.end(); ++iter)
    {
        TextSplash* textSplash = *iter;
        if (textSplash->m_age > textSplash->m_maxAge)
        {
            delete textSplash;
            iter = m_textSplashes.erase(iter);
        }
        if (iter == m_textSplashes.end())
        {
            break;
        }
    }
}
