#pragma once

#include "Internal/Common.hpp"

// TEMP: FIXME
#include "../Graphics/Internal/Common.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace zrn
{

// Time in seconds
using Time = double;

class Timestep;

} // namespace zrn