#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector2Int.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RGBA.hpp"
#include "Engine/Renderer/AABB2.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/Logging.hpp"
#include "Engine/Core/ProfilingUtils.h"
#include "Game/TheGame.hpp"
#include "Engine/Core/Event.hpp"

typedef unsigned char uchar;

#define UNUSED(x) (void)(x);

extern bool g_isQuitting;
extern bool g_renderDebug;
extern int g_frameNumber;
extern Event<float> OnUpdate;