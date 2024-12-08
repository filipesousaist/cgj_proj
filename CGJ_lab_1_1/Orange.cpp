#pragma once

#include "Orange.h"
#include <string>
#include <math.h>
#include "constants.h"
#include "Utils.h"
#include "MathUtils.h"
#include <iostream>

using namespace std;
using namespace Utils;
using namespace MathUtils;

const int ORANGE_RADIUS = 2;
const float INITIAL_SPEED = 0.005f;
const float SPEED_INCREASE = 0.005f;
const float ORANGE_MAX_SPEED = 0.003f; //0.025f;

vector<float*> Orange::spawnPoints;

Orange::Orange(Map* map) {
	spawnRandomly(INITIAL_SPEED);
	addParts();
	this->map = map;
}

void Orange::addSpawnPoint(float x, float z) {
	spawnPoints.push_back(new float[] {x, z});
}

void Orange::spawnRandomly(float newSpeed) {
	float* spawnPoint = Orange::spawnPoints.at(randInt(0, spawnPoints.size() - 1));
	x = spawnPoint[0];
	y = ORANGE_RADIUS;
	z = spawnPoint[1];
	speed = newSpeed;
	angle = modAngle(randFloat(0, 360));
	rollAngle = 0;
}

void Orange::reset() {
	spawnRandomly(INITIAL_SPEED);
}

float Orange::movePosition(int deltaTime) {
	float angleRad = angle * DEG_TO_RAD;
	float deltaPos = speed * deltaTime;
	x += cos(angleRad) * deltaPos;
	z -= sin(angleRad) * deltaPos;

	if (abs(x) >= 50 || abs(z) >= 50) {
		spawnRandomly(fminf(speed + SPEED_INCREASE, ORANGE_MAX_SPEED));
	}

	return deltaPos;
}

void Orange::roll(int deltaTime, float deltaPos) {
	rollAngle = modAngle(rollAngle + RAD_TO_DEG * deltaPos / ORANGE_RADIUS);
}

void Orange::move(int deltaTime) {
	float deltaPos = movePosition(deltaTime);
	roll(deltaTime, deltaPos);
}

void Orange::addParts() {
	MyMesh amesh;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 5.0f;
	int texIndices[] = { ORANGE_TEX, NO_TEX };
	bool mergeTextureWithColor = false;

	amesh = createSphere(2, 20);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);
	
	this->addPart(amesh);
	
	// DEBUG
	/* 
	amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, NULL, mergeTextureWithColor);
	this->addPart(amesh,
		2.5f, 0, 0,
		1.0f, 0.25f, 0.25f);
	*/
}

void Orange::handleCollision() {
	Car* car = map->getCar();
	float collisionMinDistance = car->getColliderSize() / 2 + ORANGE_RADIUS;

	if (abs(car->getX() - getX()) <= collisionMinDistance &&
		abs(car->getZ() - getZ()) <= collisionMinDistance) {
		car->reset();
		car->loseLife();
		map->resetOranges();
	}
}