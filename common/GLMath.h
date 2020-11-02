#pragma once

#define GLM_FORCE_RADIANS

//TODO mingw64 auto alignment not working
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
