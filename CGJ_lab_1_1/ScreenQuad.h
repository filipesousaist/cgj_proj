#pragma once
#include "Object.h"

class ScreenQuad : public Object
{
public:
	ScreenQuad::ScreenQuad();
	void resize(float scaleX, float scaleY, float windowWidth, float windowHeight);
};

