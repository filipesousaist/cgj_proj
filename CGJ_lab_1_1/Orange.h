#pragma once
#include <string>
#include <vector>

#include "Object.h"
#include "Map.h"

class Orange : public Object
{
private:
	static vector<float*> spawnPoints;

	float speed;

	Map* map;

	void spawnRandomly(float initialSpeed);

	float movePosition(int deltaTime);
	void roll(int deltaTime, float deltaPos);
	void move(int deltaTime);

	void addParts();

public:
	Orange::Orange(Map* map);

	static void addSpawnPoint(float x, float z);

	void reset();
	
	void handleCollision();
};

