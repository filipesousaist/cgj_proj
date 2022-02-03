#pragma once

#include <vsShaderLib.h>

#include "Object.h"
#include "Lives.h"
#include "Map.h"

class Map;

#ifndef CAR_H
#define CAR_H

const float ACC = 1e-5f;
const float ANG_SPEED = 1.2e-2f;
const float MAX_SPEED = 2.5e-3f;

#endif // CAR_H

class Car : public Object
{
private:
	VSShaderLib* shader;

	float colliderSize;

	float speed;
	float angSpeed;

	float accTang;
	float accNorm;

	bool accFront = false;
	bool accBack = false;
	bool turningLeft = false;
	bool turningRight = false;

	bool canMoveBack = true;
	bool canMoveFront = true;

	float spawnX;
	float spawnZ;
	float spawnAngle;

	Map* map;
	Lives* lives;

	void movePosition(int deltaTime);
	void moveAngle(int deltaTime);

	void move(int deltaTime);

	void addSpotLights();

public:
	Car::Car(VSShaderLib* shader, float sizeX, float sizeZ, Map* map, Lives* lives);

	vector<MyMesh> carMeshes;

	void turnLeft(bool active);
	void turnRight(bool active);

	void accelerate(bool active);
	void accelerateBack(bool active);

	void stop();
	void reset();
	void setSpawnPoint(float x, float z, float angle);
	void moveSpotLights();

	void loseLife();

	float getSpeed() {
		return speed;
	}
	float getAngularSpeed() {
		return angSpeed;
	}

	float getColliderSize() {
		return colliderSize;
	}
};