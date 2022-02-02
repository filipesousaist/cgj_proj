#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "constants.h"
#include "Map.h"
#include "Candle.h"
#include "Cheerio.h"
#include "Butter.h"
#include "Orange.h"
#include "Pawn.h"
#include "Table.h"
#include "Tree.h"

using namespace std;

int NUM_LIVES = 5;

Map::Map(VSShaderLib* shader) {
	ifstream inFile("map.txt");
	vector<string> lines;
	string line;
	
	while (getline(inFile, line))
		lines.push_back(line);

	createEntities(lines, shader);
}

void Map::createEntities(vector<string> lines, VSShaderLib* shader) {
	int width = lines[0].length();
	int height = lines.size();
	
	gameObjects.push_back(new Table(width, height));

	createCar(shader);

	pointLightPositions = new float* [NUM_POINT_LIGHTS];

	float offsetX = 0.5f * (1 - width);
	float offsetZ = 0.5f * (1 - height);

	for (int z = 0; z < height; z++) {
		string line = lines[z];
		for (int x = 0; x < width; x++)
			createEntity(line[x], x + offsetX, z + offsetZ);
	}

	spawnOranges();
}

void Map::createEntity(char c, float x, float z) {
	switch (c)
	{
	case 'b':
		createButter(x, z, false); break;
	case 'B':
		createButter(x, z, true); break;
	case 'C':
		createCandle(x, z); break;
	case 'o':
		createCheerio(x, z); break;
	case 'O':
		Orange::addSpawnPoint(x, z); break;
	case 'P':
		createPawn(x, z); break;
	case 'T':
		createTree(x, z); break;
	case 'S':
		spawnCar(x, z); break;
	default:
		break;
	}
}

void Map::createButter(float x, float z, bool flipped) {
	float sizeX = flipped ? 1.5f : 3.0f;
	float sizeZ = flipped ? 3.0f : 1.5f;
	gameObjects.push_back(new Butter(x, 0, z, sizeX, sizeZ, car));
}

void Map::createCandle(float x, float z) {
	static int i = 0;
	pointLightPositions[i ++] = new float[] { x, 8.0f, z, 1.0f };
	
	gameObjects.push_back(new Candle(x, 0, z, 6.5f));
}

void Map::createCar(VSShaderLib* shader) {
	lives = new Lives(-0.6f, 0.8f, 0.08f, NUM_LIVES);
	gameObjects.push_back(car = new Car(shader, 3.2f, 1.0f, lives));
}

void Map::createCheerio(float x, float z) {
	gameObjects.push_back(new Cheerio(x, 0.1f, z, 0.4f, car));
}

void Map::createPawn(float x, float z) {
	gameObjects.push_back(new Pawn(x, z));
}

void Map::createTree(float x, float z) {
	gameObjects.push_back(new Tree(x, 2.5f, z));
}

void Map::spawnCar(float x, float z) {
	car->setSpawnPoint(x, z);
	car->reset();
}

void Map::spawnOranges() {
	for (int o = 0; o < NUM_ORANGES; o ++)
		gameObjects.push_back(new Orange(car));
}

