#include "MathUtils.h"
#include <math.h>
#include <limits>
#include <algorithm>

using namespace std;

int MathUtils::clampI(int n, int minN, int maxN) {
	return max(minN, min(maxN, n));
}

float MathUtils::clampF(float n, float minN, float maxN) {
	return fmaxf(minN, fminf(maxN, n));
}

double MathUtils::clampD(double n, double minN, double maxN) {
	return fmax(minN, fmin(maxN, n));
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

int MathUtils::randInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}
