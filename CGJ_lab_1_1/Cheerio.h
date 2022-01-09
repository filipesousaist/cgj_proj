#pragma once
#include "Object.h"
class Cheerio : public Object
{
public:
	Cheerio::Cheerio(float x, float y, float z, float radius);
	
	float getRadius() {
		return radius;
	};

private:
	float radius;
};


