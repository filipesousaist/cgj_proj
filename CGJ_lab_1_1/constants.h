#pragma once

#include <math.h>
#include <GL/glew.h>

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
ORANGE_TEX = 3,
ORANGE_NORM = 4,
TREE_TEX = 5,
PARTICLE_TEX = 6,
LIFE_TEX = 7,
SKY_TEX = 8,
SUGAR_TEX = 9;

const int GL_TEXTURES[] = { GL_TEXTURE0, GL_TEXTURE1 };

const int NUM_POINT_LIGHTS = 6;

const int NUM_SPOT_LIGHTS = 2;

const int NUM_ORANGES = 6;

const int MAX_PARTICLES = 1500;