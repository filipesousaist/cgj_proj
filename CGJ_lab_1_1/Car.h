#pragma once
#include "Object.h"

class Car : public Object
{
private:
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

	void move(int deltaTime);
	void addParts();

	void addBody();
	void addWheels();
	
public:
	Car::Car();

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

	string getType() { return "CAR"; }
};