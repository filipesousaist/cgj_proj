#pragma once
#include "Object.h"
#include <vsShaderLib.h>

class Car : public Object
{
private:
	VSShaderLib* shader;

	float speed;
	float angSpeed;

	float accTang;
	float accNorm;

	bool accFront = false;
	bool accBack = false;
	bool turningLeft = false;
	bool turningRight = false;
	bool colliding = false;

	void movePosition(int deltaTime);
	void moveAngle(int deltaTime);
	void moveSpotLights();
	void moveCollision(int deltaTime);

	void move(int deltaTime);
	void addParts();

	void addBody();
	void addWheels();
	void addSpotLights();
	
public:
	Car::Car(VSShaderLib* shader);

	void accelerate(bool active);
	void accelerateBack(bool active);

	void turnLeft(bool active);
	void turnRight(bool active);

	float getSpeed() {
		return speed;
	}
	float getAngularSpeed() {
		return angSpeed;
	}

	void isColliding(bool collide);
};