#pragma once

#include <string>
#include <sstream>
#include <math.h>
#include <AVTmathLib.h>

#include "Car.h"
#include "constants.h"
#include "Utils.h"
#include "MathUtils.h"
#include <meshFromAssimp.h>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/scene.h"

using namespace std;
using namespace Utils;
using namespace MathUtils;

char model_dir[50];  //initialized by the user input at the console
extern const aiScene* scene;

float SPOT_LIGHT_POS[NUM_SPOT_LIGHTS][4]{
	{1.0f, 0.5f, -0.25f, 1.0f},
	{1.0f, 0.5f, 0.25f, 1.0f}
};

Car::Car(VSShaderLib* shader, float sizeX, float sizeZ, Map* map, Lives* lives) {
	angle = 0;
	rollAngle = -90;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;

	colliderSize = (sizeX + sizeZ) / 2;

	this->shader = shader;
	this->map = map;
	this->lives = lives;

	char* filename = "cars";

	std::string filepath;

	std::ostringstream oss;
	oss << "img/" << filename << "/" << filename << ".obj";
	filepath = oss.str();   //path of OBJ file in the VS project

	if (!Import3DFromFile(filepath))
		exit(1);

	strcat(model_dir, "img/");
	strcat(model_dir, filename);
	strcat(model_dir, "/");

	carMeshes = createMeshFromAssimp(scene);

	for (MyMesh amesh : carMeshes) {
		addPart(amesh, 0, 0.5f, 0,
			0.02f, 0.02f, 0.02f,
			90, 0, 1, 0);
	}
	addSpotLights();
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
	movePosition(deltaTime);
	moveAngle(deltaTime);

	//moveSpotLights();
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

	if (abs(x) > map->getWidth() * 0.5f || abs(z) > map->getHeight() * 0.5f) {
		reset();
		loseLife();
		map->resetOranges();
	}
}

void uniformSpotLightPos(VSShaderLib* shader, string name, int index, float pos_model[4]) {
	float pos_viewModel[4];
	multMatrixPoint(VIEW, pos_model, pos_viewModel);

	stringstream ss;
	ss.str("");
	ss << name << "[" << index << "]";
	GLint uniformId = glGetUniformLocation(shader->getProgramIndex(), ss.str().c_str());
	glUniform3fv(uniformId, 1, pos_viewModel);
}

void Car::moveSpotLights() {
	float angleRad = angle * DEG_TO_RAD;

	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		pushMatrix(MODEL);

		translate(MODEL, getX(), getY(), getZ());
		rotate(MODEL, getAngle(), 0, 1, 0);
		translate(MODEL, SPOT_LIGHT_POS[i][0], SPOT_LIGHT_POS[i][1], SPOT_LIGHT_POS[i][2]);

		// Spotlight position
		float pos[4];
		multMatrixPoint(MODEL, new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f }, pos);
		uniformSpotLightPos(shader, "spotLightPos", i, pos);

		// Spotlight end position
		pos[0] += cos(angleRad);
		pos[2] -= sin(angleRad);
		uniformSpotLightPos(shader, "spotLightEndPos", i, pos);

		popMatrix(MODEL);
	}
}

void Car::turnLeft(bool active) {
	turningLeft = active;
}

void Car::turnRight(bool active) {
	turningRight = active;
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
	x = spawnX;
	z = spawnZ;

	angle = spawnAngle;
	rollAngle = -90;

	speed = 0;
	angSpeed = 0;

	accTang = accNorm = 0;
}

void Car::setSpawnPoint(float x, float z, float angle)
{
	spawnX = x;
	spawnZ = z;
	spawnAngle = angle;
}

void Car::loseLife() {
	lives->loseLife();
	reset();
}


