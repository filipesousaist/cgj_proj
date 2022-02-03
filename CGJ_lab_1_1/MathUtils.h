#pragma once
namespace MathUtils
{
	int clampI(int n, int minN, int maxN);
	float clampF(float n, float minN, float maxN);
	double clampD(double n, double minN, double maxN);
	float modAngle(float angle);
	float randFloat(float min, float max);
	int randInt(int min, int max);
};

