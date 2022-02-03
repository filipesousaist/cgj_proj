#pragma once

#include <vsShaderLib.h>

#include "Object.h"
#include "Car.h"
#include "Checkpoint.h"
#include "Coin.h"
#include "Lives.h"
#include "Table.h"

#include "Orange.h"

class Car;
class Coin;
class Checkpoint;
class Goal;
class Orange;

class Map
{
private:
	int width;
	int height;

	vector<Object*> gameObjects;
	vector<Checkpoint*> checkpoints;
	vector<Coin*> coins;
	vector<Orange*> oranges;
	Goal* goal;
	Car* car;
	Table* table;
	Lives* lives;
	float** pointLightPositions;

	map<string, int> numCollectedCoins;
	int score;

	int lap;
	int numReachedCheckpoints;
	bool won;

	void createEntities(vector<string> lines, VSShaderLib* shader);
	void createEntity(char c, float x, float z);
	void createButter(float x, float z, bool flipped);
	void createCandle(float x, float z);
	void createCar(VSShaderLib* shader);
	void createCheckpoint(float x, float z, float angle);
	void createCheerio(float x, float z);
	void createMirrorCube(float x, float z);
	void createPawn(float x, float z);
	void createTree(float x, float z);

	void createGoldCoin(float x, float z);
	void createSilverCoin(float x, float z);
	void createCopperCoin(float x, float z);

	void spawnCar(float x, float z);
	void spawnOranges();

	void resetCheckpoints();
	void resetCoins();
	void showCoins();

public:
	Map::Map(VSShaderLib* shader);

	int getWidth() { return width; }
	int getHeight() { return height; }

	vector<Object*> getGameObjects() { return gameObjects; };
	Car* getCar() { return car; };
	Table* getTable() { return table; };
	Lives* getLives() { return lives; };
	float** getPointLightPositions() { return pointLightPositions; };

	void resetOranges();
	
	void collectCoin(Coin* coin);
	int getScore() { return score; }

	void reachCheckpoint();

	int getNumCheckpoints();
	int getNumReachedCheckpoints() { return numReachedCheckpoints; }

	int getLap() { return lap; }
	int getNumLaps();

	bool hasWon() { return won; }

	void reset();
};

