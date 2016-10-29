#pragma once
#include "Engine\Renderer\2D\Sprite.hpp"

class GameMode
{
public:
    GameMode(const std::string& arenaBackgroundImage = "DefaultBackground");
    virtual ~GameMode();

private:
    Sprite m_arenaBackground;
};