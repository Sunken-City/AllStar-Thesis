#pragma once
#include <string>
#include "Engine\Math\Transform2D.hpp"
#include "Engine\Core\Memory\UntrackedAllocator.hpp"
#include <vector>
#include "Engine\Renderer\RGBA.hpp"
#include "..\TheGame.hpp"

class TextRenderable2D;

//-----------------------------------------------------------------------------------
class TextSplash
{
public:
    static TextSplash* CreateTextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color = RGBA::WHITE, int orderLayer = TheGame::TEXT_PARTICLE_LAYER);
    static void Update(float deltaSeconds);
    static void Cleanup();

    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    TextSplash(const std::string& text, const Transform2D& spawnTransform, const Vector2& velocity, RGBA color = RGBA::WHITE, int orderLayer = TheGame::TEXT_PARTICLE_LAYER);
    ~TextSplash();

    //VARIABLES/////////////////////////////////////////////////////////////////////
    static std::vector<TextSplash*, UntrackedAllocator<TextSplash*>> m_textSplashes;

    TextRenderable2D* m_textRenderable;
    Vector2 m_velocity;
    float m_age = 0.0f;
    float m_maxAge = 1.0f;
};