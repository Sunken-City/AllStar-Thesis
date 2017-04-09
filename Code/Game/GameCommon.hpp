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
#include "Engine/Core/Events/Event.hpp"
#include "Engine/Audio/Audio.hpp"
#include <xstddef>

typedef unsigned char uchar;

#define UNUSED(x) (void)(x);

extern bool g_isQuitting;
extern bool g_isGamePaused;
extern bool g_renderDebug;
extern int g_frameNumber;
extern Event<float> OnUpdate;

extern bool g_spawnEnemies;
extern bool g_spawnCrates;
extern bool g_spawnGeometry;
extern bool g_nearlyInvulnerable;
extern bool g_spawnWithDebugLoadout;
extern bool g_disableMusic;

extern const size_t gEffectTimeUniform;
extern const size_t gWipeColorUniform;
extern const size_t paletteOffsetUniform;
extern const size_t warpPositionsUniforms;
extern const size_t vortexRadiiUniforms;