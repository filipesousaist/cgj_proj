#pragma once
#include "Object.h"

class Car : public Object
{
private:
	float x;
	float y;
	float z;
	float angle;

	float speed;
	float angSpeed;

	float accTang;
	float accNorm;

	bool accFront = false;
	bool accBack = false;
	bool turningLeft = false;
	bool turningRight = false;

	void movePosition(int deltaTime);
	void moveAngle(int deltaTime);
	
public:
	Car::Car();
	void Car::addBody();
	void Car::addWheels();
	void move(int deltaTime);
	void addParts();

	void accelerate(bool active);
	void accelerateBack(bool active);

	void turnLeft(bool active);
	void turnRight(bool active);

	float getX() {
		return x;
	};
	float getY() {
		return y;
	};
	float getZ() {
		return z;
	};
	float getAngle() {
		return angle;
	}

	float getSpeed() {
		return speed;
	}

	float getAngularSpeed() {
		return angSpeed;
	}
};