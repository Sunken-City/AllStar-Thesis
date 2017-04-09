#include "Game\GameCommon.hpp"

int g_frameNumber  = 0;
bool g_renderDebug = true;
bool g_isGamePaused = false;

bool g_spawnEnemies             = true;
bool g_spawnCrates              = true;
bool g_spawnGeometry            = true;
bool g_nearlyInvulnerable       = false;
bool g_spawnWithDebugLoadout    = true;
bool g_disableMusic             = true;

const size_t gEffectTimeUniform = std::hash<std::string>{}("gEffectTime");
const size_t gWipeColorUniform = std::hash<std::string>{}("gWipeColor");
const size_t paletteOffsetUniform = std::hash<std::string>{}("PaletteOffset");
const size_t warpPositionsUniforms = std::hash<std::string>{}("gWarpPositions[0]");
const size_t vortexRadiiUniforms = std::hash<std::string>{}("gVortexRadii[0]");
