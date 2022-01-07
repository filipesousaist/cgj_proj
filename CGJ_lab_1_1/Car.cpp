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

Car::Car(VSShaderLib* shader) {
	angle = 0;
	rollAngle = 0;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;

	this->shader = shader;

	addParts();
}

void Car::addParts() {
	addBody();
	addWheels();
	addSpotLights();
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
		this->addPart(
				amesh,
				1.0f, 0.5f, 0.25 * zSign);
	}
}

void Car::move(int deltaTime) {
	movePosition(deltaTime);
	moveAngle(deltaTime);
	moveSpotLights(deltaTime);
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

	float accDrag = speed * 3e-4f;
	speed += (ACC * accMult - accDrag) * deltaTime;
}

void Car::moveSpotLights(int deltaTime) {
	
	float angleRad = angle * DEG_TO_RAD;
	float spotLightDir[4]{ cos(angleRad), 0.0f, -sin(angleRad), 1.0f };

	float dirX = cos(angleRad);
	float dirZ = sin(angleRad);

	float perpX = cos(angleRad + 3.14f / 2);
	float perpZ = sin(angleRad + 3.14f / 2);

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		pushMatrix(MODEL);
		translate(MODEL, getX(), getY(), getZ());
		rotate(MODEL, getAngle(), 0, 1, 0);
		translate(MODEL, spotLightPos[i][0], spotLightPos[i][1], spotLightPos[i][2]);
		
		float res[4];
		multMatrixPoint(MODEL, new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f }, res);
		multMatrixPoint(VIEW, res, res);   //lightPos definido em World Coord so is converted to eye space
		stringstream ss;
		ss.str("");
		ss << "sl_pos[" << i << "]";
		GLint slPos_uniformId = glGetUniformLocation(shader->getProgramIndex(), ss.str().c_str());
		glUniform4fv(slPos_uniformId, 1, res);
		popMatrix(MODEL);

		pushMatrix(MODEL);
		//rotate(MODEL, getAngle(), 0, 1, 0);
		multMatrixPoint(MODEL, new float[4]{ getX() + 1.0f, getY(), getZ(), 1.0f }, res);
		multMatrixPoint(VIEW, res, res);
		
		ss.str("");
		ss << "coneDir[" << i << "]";
		GLint coneDir_uniformId = glGetUniformLocation(shader->getProgramIndex(), ss.str().c_str());
		glUniform4fv(coneDir_uniformId, 1, res);

		popMatrix(MODEL);
	}
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
