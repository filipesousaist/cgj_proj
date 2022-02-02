#pragma once

#include <vsShaderLib.h>

#include "Object.h"
#include "Car.h"
#include "Lives.h"

class Map
{
private:
	vector<Object*> gameObjects;
	Car* car;
	Lives* lives;
	float** pointLightPositions;

	void createEntities(vector<string> lines, VSShaderLib* shader);
	void createEntity(char c, float x, float z);
	void createButter(float x, float z, bool flipped);
	void createCandle(float x, float z);
	void createCar(VSShaderLib* shader);
	void createCheerio(float x, float z);
	void createMirrorCube(float x, float z);
	void createPawn(float x, float z);
	void createTree(float x, float z);

	void spawnCar(float x, float z);
	void spawnOranges();

public:
	Map::Map(VSShaderLib* shader);

	vector<Object*> getGameObjects() { return gameObjects; };
	Car* getCar() { return car; };
	Lives* getLives() { return lives; };
	float** getPointLightPositions() { return pointLightPositions; };

};

