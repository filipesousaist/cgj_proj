#pragma once
#include "Object.h"
#include <vsShaderLib.h>

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
	bool colliding = false;

	bool canMoveBack = true;
	bool canMoveFront = true;

	void movePosition(int deltaTime);
	void moveAngle(int deltaTime);
	void moveSpotLights();
	void moveCollision(int deltaTime);

	void move(int deltaTime);
	void addParts(float sizeX, float sizeZ);

	void addBody(float sizeX, float sizeZ);
	void addWheels();
	void addSpotLights();
	
public:
	Car::Car(VSShaderLib* shader, float sizeX, float sizeZ);

	void accelerate(bool active);
	void accelerateBack(bool active);
	
	void stop();

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


	void isColliding(bool collide);
};