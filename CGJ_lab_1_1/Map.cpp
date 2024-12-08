#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "constants.h"
#include "Map.h"
#include "Butter.h"
#include "Candle.h"
#include "Cheerio.h"
#include "MirrorCube.h"
#include "Orange.h"
#include "Pawn.h"
#include "Table.h"
#include "Tree.h"

#include "GoldCoin.h"
#include "SilverCoin.h"
#include "CopperCoin.h"

using namespace std;

const int NUM_LIVES = 5;
const int NUM_LAPS = 3;

const string COIN_TYPES[] = { "Copper", "Silver", "Gold" };

Map::Map(VSShaderLib* shader) {
	ifstream inFile("map.txt");
	vector<string> lines;
	string line;
	
	while (getline(inFile, line))
		lines.push_back(line);

	createEntities(lines, shader);
}

void Map::createEntities(vector<string> lines, VSShaderLib* shader) {
	width = lines[0].length();
	height = lines.size();
	
	table = (new Table(width, height));

	numReachedCheckpoints = 0;
	lap = 1;

	createCar(shader);

	pointLightPositions = new float* [NUM_POINT_LIGHTS];

	float offsetX = 0.5f * (1 - width);
	float offsetZ = 0.5f * (1 - height);

	for (int z = 0; z < height; z++) {
		string line = lines[z];
		for (int x = 0; x < width; x++)
			createEntity(line[x], x + offsetX, z + offsetZ);
	}
	reset();
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
	case 'M':
		createMirrorCube(x, z); break;
	case 'o':
		createCheerio(x, z); break;
	case 'O':
		Orange::addSpawnPoint(x, z); break;
	case 'P':
		createPawn(x, z); break;
	case 'S':
		spawnCar(x, z); break;
	case 'T':
		createTree(x, z); break;
	case '>':
		createCheckpoint(x, z, 0); break;
	case '^':
		createCheckpoint(x, z, 90); break;
	case '<':
		createCheckpoint(x, z, 180); break;
	case 'v': 
		createCheckpoint(x, z, 270); break;
	case '1':
		createCopperCoin(x, z); break;
	case '2':
		createSilverCoin(x, z); break;
	case '3':
		createGoldCoin(x, z); break;
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
	car = new Car(shader, 3.2f, 1.0f, this, lives);
}

void Map::createCheckpoint(float x, float z, float angle) {
	Checkpoint* checkpoint = new Checkpoint(x, z, angle, this);
	gameObjects.push_back(checkpoint);
	checkpoints.push_back(checkpoint);
}

void Map::collectCoin(Coin* coin)
{
	numCollectedCoins[coin->getType()] ++;
	score += coin->getValue();
}

void Map::reachCheckpoint() {
	numReachedCheckpoints ++;
	if (numReachedCheckpoints == checkpoints.size()) {
		if (lap == NUM_LAPS)
			won = true;
		else {
			lap ++;
			numReachedCheckpoints = 0;
			showCoins();
			resetCheckpoints();
		}
	}
}

void Map::resetCheckpoints()
{
	for (Checkpoint* checkpoint : checkpoints)
		checkpoint->setNotReached();
}

int Map::getNumCheckpoints() {
	return checkpoints.size();
}

int Map::getNumLaps() {
	return NUM_LAPS;
}

void Map::createCheerio(float x, float z) {
	gameObjects.push_back(new Cheerio(x, 0.1f, z, 0.4f, car));
}

void Map::createMirrorCube(float x, float z) {
	gameObjects.push_back(new MirrorCube(x, z));
}

void Map::createPawn(float x, float z) {
	gameObjects.push_back(new Pawn(x, z));
}

void Map::createTree(float x, float z) {
	gameObjects.push_back(new Tree(x, 2.5f, z));
}

void Map::createGoldCoin(float x, float z)
{
	GoldCoin* goldCoin = new GoldCoin(x, z, this);
	gameObjects.push_back(goldCoin);
	coins.push_back(goldCoin);
}

void Map::createSilverCoin(float x, float z)
{
	SilverCoin* silverCoin = new SilverCoin(x, z, this);
	gameObjects.push_back(silverCoin);
	coins.push_back(silverCoin);
}

void Map::createCopperCoin(float x, float z)
{
	CopperCoin* copperCoin = new CopperCoin(x, z, this);
	gameObjects.push_back(copperCoin);
	coins.push_back(copperCoin);
}

void Map::resetCoins() {
	for (string type : COIN_TYPES)
		numCollectedCoins[type] = 0;
	showCoins();
}

void Map::showCoins() {
	for (Coin* coin : coins)
		coin->show();
}

void Map::spawnCar(float x, float z) {
	float angle = 0;
	car->setSpawnPoint(x, z, angle);
	goal = new Goal(x, z, angle, this);
	gameObjects.push_back(goal);
	checkpoints.push_back(goal);
}

void Map::spawnOranges() {
	for (int o = 0; o < NUM_ORANGES; o++) {
		Orange* orange = new Orange(this);
		gameObjects.push_back(orange);
		oranges.push_back(orange);
	}
}

void Map::resetOranges() {
	for (Orange* orange : oranges)
		orange->reset();
}

void Map::reset() {
	score = 0;
	resetCoins();
	resetCheckpoints();
	numReachedCheckpoints = 0;
	lap = 1;
	won = false;
	car->setSpawnPoint(goal->getX(), goal->getZ(), goal->getAngle());
	lives->reset();
	car->reset();
}

