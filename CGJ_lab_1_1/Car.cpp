#pragma once

#include "Car.h"
#include <string>
#include <sstream>
#include <math.h>
#include "constants.h"
#include "Utils.h"
#include "MathUtils.h"
#include <AVTmathLib.h>

using namespace std;
using namespace Utils;
using namespace MathUtils;

const float ACC = 1e-5f;
const float ANG_SPEED = 2e-2f;
const float MAX_SPEED = 3e-3f;

float spotLightPos[NUM_SPOT_LIGHTS][4]{
	{1.0f, 0.5f, -0.25f, 1.0f},
	{1.0f, 0.5f, 0.25f, 1.0f}
};

Car::Car(VSShaderLib* shader, float sizeX, float sizeZ) {
	angle = 0;
	rollAngle = 0;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;

	colliderSize = (sizeX + sizeZ) / 2;

	this->shader = shader;

	addParts(sizeX, sizeZ);
}

void Car::addParts(float sizeX, float sizeZ) {
	addBody(sizeX, sizeZ);
	addWheels();
	addSpotLights();
}

void Car::addBody(float sizeX, float sizeZ) {
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
		sizeX, 0.5f, sizeZ);

	// Cockpit
	addPart(amesh,
		-0.5f,  0.65f, -0.3f,
		0.4f * sizeX, 0.4f, 0.6f * sizeZ);
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

void Car::addSpotLights() {
	MyMesh amesh;

	float amb[] = { 0.8f, 0.4f, 0.0f, 1.0f };
	float diff[] = { 0.6f, 0.6f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	amesh = createSphere(0.1f, 20);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	float signs[]{ -1, 1 };

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		float zSign = signs[i];
		this->addPart(amesh,
			1.0f, 0.5f, 0.25 * zSign,
			0.5f, 1.0f, 1.0f);
	}
}

void Car::move(int deltaTime) {
	if (!colliding) {
		movePosition(deltaTime);
		moveAngle(deltaTime);
	}
	else {
		moveCollision(deltaTime);
	}

	moveSpotLights();

}
void Car::moveCollision(int deltaTime) {
	int accMult = 0;
	if (accFront)
		accMult++;
	if (accBack)
		accMult--;

	float angleRad = angle * DEG_TO_RAD;

	float deltaPos = speed * deltaTime + 0.5f * ACC * accMult * deltaTime * deltaTime;
	x -= cos(angleRad) * deltaPos;
	z += sin(angleRad) * deltaPos;

	speed = -speed / 2;
}

void Car::moveAngle(int deltaTime) {
	int turnMult = 0;
	if (turningLeft)
		turnMult++;
	if (turningRight)
		turnMult--;
	
	float speedFraction = speed / MAX_SPEED;
	angSpeed = ANG_SPEED * speedFraction * turnMult;

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

	float accDrag = speed * 3e-4f;
	speed += (ACC * accMult - accDrag) * deltaTime;
}

void Car::moveSpotLights() {
	
	float angleRad = angle * DEG_TO_RAD;

	float dirX = cos(angleRad);
	float dirZ = sin(angleRad);

	float perpX = cos(angleRad + 3.14f / 2);
	float perpZ = sin(angleRad + 3.14f / 2);

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		pushMatrix(MODEL);

		translate(MODEL, getX(), getY(), getZ());
		rotate(MODEL, getAngle(), 0, 1, 0);
		translate(MODEL, spotLightPos[i][0], spotLightPos[i][1], spotLightPos[i][2]);
		
		// Spotlight position

		float res_m[4];
		float res_vm[4];
		multMatrixPoint(MODEL, new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f }, res_m);
		multMatrixPoint(VIEW, res_m, res_vm);

		stringstream ss;
		ss.str("");
		ss << "spotLightPos[" << i << "]";
		GLint slPos_uniformId = glGetUniformLocation(shader->getProgramIndex(), ss.str().c_str());
		glUniform3fv(slPos_uniformId, 1, res_vm);

		// Spotlight end position

		res_m[0] += cos(angleRad);
		res_m[2] -= sin(angleRad);
		multMatrixPoint(VIEW, res_m, res_vm);
		
		ss.str("");
		ss << "spotLightEndPos[" << i << "]";
		GLint coneDir_uniformId = glGetUniformLocation(shader->getProgramIndex(), ss.str().c_str());
		glUniform3fv(coneDir_uniformId, 1, res_vm);

		popMatrix(MODEL);
	}
}

void Car::accelerate(bool active) {
	if (canMoveFront)
		accFront = active;
	canMoveBack = true;
}

void Car::accelerateBack(bool active) {
	if (canMoveBack)
		accBack = active;
	canMoveFront = true;
}

void Car::stop() {
	if (speed > 0)
		canMoveFront = false;
	else if (speed < 0)
		canMoveBack = false;
	speed = 0;
	accFront = false;
	accBack = false;
	turningLeft = false;
	turningRight = false;
}

void Car::reset() {
	angle = 0;
	rollAngle = 0;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;

	x = 0;
	z = 0;

}

void Car::turnLeft(bool active) {
	turningLeft = active;
}

void Car::turnRight(bool active) {
	turningRight = active;
}

void Car::isColliding(bool collide) {
	colliding = collide;
}
