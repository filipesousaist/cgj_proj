#include "Car.h"
#include <string>
#include <math.h>

const double PI = 4 * atan(1);
const float ACC = 1e-5f;
const float ANG_SPEED = 1e-3f;
const float MAX_SPEED = 1e-3f;

using namespace std;

Car::Car() {
	x = y = z = 0;
	angle = 0;

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
	int texcount = 0;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texcount);

	this->addPart(amesh,
		-1.0f, 0.25f, -0.5f,
		2.0f, 0.5f, 1.0f);

	// Cockpit
	this->addPart(amesh,
		-0.5f,  0.65f, -0.3f,
		0.8f, 0.4f, 0.6f);
}

void Car::addWheels() {
	MyMesh amesh;

	float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	float diff[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int texcount = 0;

	amesh = createTorus(0.05f, 0.25f, 20, 12);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texcount);

	float signs[]{ -1, 1 };

	for (int xi = 0; xi < 2; xi++) {
		float xSign = signs[xi];
		for (int zi = 0; zi < 2; zi++) {
			float zSign = signs[zi];

			this->addPart(
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
	angSpeed = ANG_SPEED * speedFraction * turnMult * deltaTime;
	

	angle += angSpeed * deltaTime;
}

void Car::movePosition(int deltaTime) {
	int accMult = 0;
	if (accFront)
		accMult ++;
	if (accBack)
		accMult --;

	float accDrag = speed * 1e-3f;
	speed += (ACC * accMult - accDrag) * deltaTime;

	float angleRad = angle * PI / 180;
	x += speed * cos(angleRad) * deltaTime;
	z -= speed * sin(angleRad) * deltaTime;
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


