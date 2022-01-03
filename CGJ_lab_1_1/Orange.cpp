#pragma once

#include "Orange.h"
#include <string>
#include <math.h>
#include "constants.h"
#include "Utils.h"
#include "MathUtils.h"

using namespace std;
using namespace Utils;
using namespace MathUtils;

const int RADIUS = 2;

Orange::Orange(float initialSpeed) {
	spawnRandomly(initialSpeed);
	addParts();
}

void Orange::spawnRandomly(float initialSpeed) {
	x = randFloat(-50, 50);
	y = RADIUS;
	z = randFloat(-50, 50);
	speed = initialSpeed;
	angle = modAngle(randFloat(0, 360));
}

void Orange::move(int deltaTime) {
	float angleRad = angle * DEG_TO_RAD;

	float deltaPos = speed * deltaTime;
	x += cos(angleRad) * deltaPos;
	z -= sin(angleRad) * deltaPos;
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

	amesh = createSphere(2, 12);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);
	
	this->addPart(amesh);
}