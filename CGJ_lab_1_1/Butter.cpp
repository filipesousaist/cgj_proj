#pragma once

#include "Butter.h"
#include "Utils.h"
#include "Car.h"

#include "AVTmathLib.h"

using namespace std;
using namespace Utils;

Butter::Butter(float x, float y, float z, float sizeX, float sizeZ, Car* car) {
	
	MyMesh amesh;

	float amb[] = { 0.1f, 0.1f, 0.0f, 1.0f };
	float diff[] = { 0.8f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 0.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	this->x = x;
	this->y = y;
	this->z = z;
	this->sizeX = sizeX;
	this->sizeZ = sizeZ;
	this->car = car;

	amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(amesh, -sizeX / 2, 0, -sizeZ / 2,
		sizeX, 0.75f, sizeZ);
}

void Butter::handleCollision() {
	float mass = 0.045f;
	bool collision = false;

	float angleRad = car->getAngle() * DEG_TO_RAD;

	float carToButter[3]{ x - car->getX(), 0, z - car->getZ() };
	float carDirection[3]{ cos(angleRad), 0, -sin(angleRad) };
	float dotProductResult = dotProduct(carToButter, carDirection);


	if (abs(car->getX() - getX()) <= (car->getColliderSize() + sizeX) / 2 &&
		abs(car->getZ() - getZ()) <= (car->getColliderSize() + sizeZ) / 2 &&
		dotProductResult * car->getSpeed() > 0)
		collision = true;

	if (collision) {
		float deltaPos = abs(car->getSpeed()) / mass;
		float deltaPosDir[3]{ carToButter[0] + cos(angleRad), 0, carToButter[2] - sin(angleRad) };
		normalize(deltaPosDir);
		x += deltaPos * deltaPosDir[0];
		z += deltaPos * deltaPosDir[2];

		car->stop();
	}
}