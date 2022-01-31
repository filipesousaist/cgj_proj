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

const int RADIUS = 2;
const float INITIAL_SPEED = 0.005f;
const float SPEED_INCREASE = 0.004f;
const float MAX_SPEED = 0.002f; //0.025f;

Orange::Orange(Car* car) {
	spawnRandomly(INITIAL_SPEED);
	addParts();
	this->car = car;
}

void Orange::spawnRandomly(float newSpeed) {
	x = randFloat(-40, 40);
	y = RADIUS;
	z = randFloat(-40, 40);
	speed = newSpeed;
	angle = modAngle(randFloat(0, 360));
	rollAngle = 0;
}

float Orange::movePosition(int deltaTime) {
	float angleRad = angle * DEG_TO_RAD;
	float deltaPos = speed * deltaTime;
	x += cos(angleRad) * deltaPos;
	z -= sin(angleRad) * deltaPos;

	if (abs(x) >= 50 || abs(z) >= 50) {
		spawnRandomly(fminf(speed + SPEED_INCREASE, MAX_SPEED));
	}

	return deltaPos;
}

void Orange::roll(int deltaTime, float deltaPos) {
	rollAngle = modAngle(rollAngle + RAD_TO_DEG * deltaPos / RADIUS);
}

void Orange::move(int deltaTime) {
	float deltaPos = movePosition(deltaTime);
	roll(deltaTime, deltaPos);
}

void Orange::addParts() {
	MyMesh amesh;

	float amb[] = { 0.8f, 0.4f, 0.0f, 1.0f };
	float diff[] = { 0.6f, 0.6f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
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
	bool collision = false;

	float angleRad = car->getAngle() * DEG_TO_RAD;

	float collisionMinDistance = car->getColliderSize() / 2 + RADIUS;

	if (abs(car->getX() - getX()) <= collisionMinDistance &&
		abs(car->getZ() - getZ()) <= collisionMinDistance)
		collision = true;

	if (collision) {
		car->reset();
		car->loseLife();
	}
}