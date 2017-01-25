#pragma once
#include <string>
#include "Engine\Math\Transform2D.hpp"
#include "Engine\Core\Memory\UntrackedAllocator.hpp"
#include <vector>

class TextRenderable2D;

//-----------------------------------------------------------------------------------
class TextSplash
{
public:
    static void CreateTextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity);
    static void Update(float deltaSeconds);
    static void Cleanup();

    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    TextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity);
    ~TextSplash();

    //VARIABLES/////////////////////////////////////////////////////////////////////
    static std::vector<TextSplash*, UntrackedAllocator<TextSplash*>> m_textSplashes;

    TextRenderable2D* m_textRenderable;
    Vector2 m_velocity;
    float m_age = 0.0f;
    float m_maxAge = 1.0f;
};