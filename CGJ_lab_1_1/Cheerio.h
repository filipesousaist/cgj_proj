#pragma once
#include "Object.h"
#include "Car.h"

class Cheerio : public Object
{
private:
	float radius;

	Car* car;

public:
	Cheerio::Cheerio(float x, float y, float z, float radius, Car* car);
	
	void handleCollision();

	float getRadius() {
		return radius;
	};
};


