#pragma once
#include "Object.h"

#include <string>

class Orange : public Object
{
private:
	float speed;

	void spawnRandomly(float initialSpeed);

	float movePosition(int deltaTime);
	void roll(int deltaTime, float deltaPos);
	void move(int deltaTime);

	void addParts();

public:
	Orange::Orange();
	
	string collides();
};

