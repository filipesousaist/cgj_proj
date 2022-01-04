#pragma once

#include "Car.h"
#include <string>
#include <math.h>
#include "constants.h"
#include "Utils.h"
#include "MathUtils.h"

using namespace std;
using namespace Utils;
using namespace MathUtils;

const float ACC = 1e-5f;
const float ANG_SPEED = 2e-2f;
const float MAX_SPEED = 3e-3f;

Car::Car() {
	angle = 0;
	rollAngle = 0;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;

	addParts();
}

void Car::addParts() {
	addBody();
	addWheels();
}

void Car::addBody() {
	float amb[] = { 0.0f, 0.6f, 0.0f, 1.0f };
	float diff[] = { 0.2f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(amesh,
		-1.0f, 0.25f, -0.5f,
		2.0f, 0.5f, 1.0f);

	// Cockpit
	addPart(amesh,
		-0.5f,  0.65f, -0.3f,
		0.8f, 0.4f, 0.6f);
}

void Car::addWheels() {
	float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	float diff[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createTorus(0.05f, 0.25f, 20, 12);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	float signs[]{ -1, 1 };

	for (int xi = 0; xi < 2; xi++) {
		float xSign = signs[xi];
		for (int zi = 0; zi < 2; zi++) {
			float zSign = signs[zi];

			addPart(
				amesh,
				0.5f * xSign, 0.25f, 0.5f * zSign,
				1.0f, 1.0f, 1.0f,
				90,
				1.0f, 0, 0
			);
		}
	}
}

void Car::move(int deltaTime) {
	movePosition(deltaTime);
	moveAngle(deltaTime);
}

void Car::moveAngle(int deltaTime) {
	int turnMult = 0;
	if (turningLeft)
		turnMult++;
	if (turningRight)
		turnMult--;
	
	float speedFraction = speed / MAX_SPEED;
	angSpeed = ANG_SPEED * speedFraction * turnMult;//*deltaTime;

	angle = modAngle(angle + angSpeed * deltaTime);
}

void Car::movePosition(int deltaTime) {

	int accMult = 0;
	if (accFront)
		accMult++;
	if (accBack)
		accMult--;

	float angleRad = angle * DEG_TO_RAD;

	float deltaPos = speed * deltaTime + 0.5f * ACC * accMult * deltaTime * deltaTime;
	x += cos(angleRad) * deltaPos;
	z -= sin(angleRad) * deltaPos;

	float accDrag = speed * 3e-4;
	speed += (ACC * accMult - accDrag) * deltaTime;
}

void Car::accelerate(bool active) {
	accFront = active;
}

void Car::accelerateBack(bool active) {
	accBack = active;
}

void Car::turnLeft(bool active) {
	turningLeft = active;
}

void Car::turnRight(bool active) {
	turningRight = active;
}
