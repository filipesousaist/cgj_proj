#pragma once

#include <math.h>

const float PI = 4 * atan(1);
const float DEG_TO_RAD = PI / 180;
const float RAD_TO_DEG = 180 / PI;

const enum class Camera {
	ORTHOGONAL, PERSPECTIVE, CAR
};

const int
	NO_TEX = -1,
	STONE_TEX = 0,
	WOOD_TEX = 1,
	CHECKERS_TEX = 2,
	ORANGE_TEX = 3;

const int GL_TEXTURES[] = { GL_TEXTURE0, GL_TEXTURE1 };

const int NUM_ORANGES = 6;