#include "MathUtils.h"
#include <math.h>
#include <limits>

using namespace std;

float MathUtils::clamp(float n, float min, float max)
{
	return fmaxf(min, fminf(max, n));
}

float MathUtils::modAngle(float angle)
{
	float newAngle = fmod(angle, 360.0f);
	return (newAngle >= 0.0f) ? newAngle : newAngle + 360.0f;
}

float MathUtils::randFloat(float min, float max)
{
	return min + ((float) rand()) / RAND_MAX * (max - min);
}
