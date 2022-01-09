#pragma once
#include "Object.h"

#include <string>
#include "Car.h"

class Orange : public Object
{
private:
	float speed;

	Car* car;

	void spawnRandomly(float initialSpeed);

	float movePosition(int deltaTime);
	void roll(int deltaTime, float deltaPos);
	void move(int deltaTime);

	void addParts();

public:
	Orange::Orange(Car* car);
	
	void handleCollision();
};

