#pragma once
#include "Object.h"

class ScreenQuad : public Object
{
private:
	float aspectRatio;
public:
	ScreenQuad::ScreenQuad(float x, float y, float scaleY, float aspectRatio,
		float colorR, float colorG, float colorB, float colorA);
	ScreenQuad::ScreenQuad(float x, float y, float scaleY, float aspectRatio);
	void resize(float windowWidth, float windowHeight);
};

