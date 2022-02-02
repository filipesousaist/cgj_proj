#pragma once

#include <vsShaderLib.h>

#include "Object.h"
#include <Lives.h>


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

	Lives* lives;

	void movePosition(int deltaTime);
	void moveAngle(int deltaTime);
	void moveSpotLights();

	void move(int deltaTime);

	void addSpotLights();

public:
	Car::Car(VSShaderLib* shader, float sizeX, float sizeZ, Lives* lives);

	vector<MyMesh> carMeshes;

	void accelerate(bool active);
	void accelerateBack(bool active);

	void stop();
	void reset();

	void loseLife();

	void turnLeft(bool active);
	void turnRight(bool active);

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