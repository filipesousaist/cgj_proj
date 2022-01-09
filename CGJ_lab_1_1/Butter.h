#pragma once
#include "Object.h"
#include "Car.h"

class Butter : public Object
{
private:
	float sizeX;
	float sizeZ;
	Car* car;
public:
	Butter::Butter(float x, float y, float z, float sizeX, float sizeZ, Car* car);

	void handleCollision();
};

