#include "Game/Entities/TextSplash.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "Game/TheGame.hpp"

std::vector<TextSplash*, UntrackedAllocator<TextSplash*>> TextSplash::m_textSplashes;

//-----------------------------------------------------------------------------------
TextSplash::TextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color /*= RGBA::WHITE*/, int orderLayer /*= TheGame::TEXT_PARTICLE_LAYER*/) 
    : m_velocity(velocity)
{
    m_textRenderable = new TextRenderable2D(text, spawnTransform, orderLayer, true);
    m_textRenderable->m_fontSize /= 3.5f;
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
//Returns a splash in case you need to edit one's properties.
TextSplash* TextSplash::CreateTextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color /*= RGBA::WHITE*/, int orderLayer /*= TheGame::TEXT_PARTICLE_LAYER*/)
{
    TextSplash* textSplash = new TextSplash(text, spawnTransform, velocity, color, orderLayer);
    textSplash->m_textRenderable->m_transform.SetScale(Vector2(1.0f));
    m_textSplashes.push_back(textSplash);
    return textSplash;
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

