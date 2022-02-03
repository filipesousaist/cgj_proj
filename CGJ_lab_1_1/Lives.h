#pragma once
#include "ScreenQuad.h"

class Lives : public ScreenQuad
{
private:
	MyMesh mesh;
	int maxN;
	int n;
public:
	Lives::Lives(float x, float y, float scaleY, int num);
	void loseLife();
	void reset();
	bool areEmpty();
};