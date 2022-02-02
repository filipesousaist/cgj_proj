#pragma once
#include <string>
#include <vector>

#include "Object.h"
#include "Car.h"

class Orange : public Object
{
private:
	static vector<float*> spawnPoints;

	float speed;

	Car* car;

	void spawnRandomly(float initialSpeed);

	float movePosition(int deltaTime);
	void roll(int deltaTime, float deltaPos);
	void move(int deltaTime);

	void addParts();

public:
	Orange::Orange(Car* car);

	static void addSpawnPoint(float x, float z);
	
	void handleCollision();
};

