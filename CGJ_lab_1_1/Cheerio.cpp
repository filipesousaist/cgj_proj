#pragma once

#include "Cheerio.h"
#include <Utils.h>
#include "Car.h"
#include "AVTmathLib.h"

using namespace std;
using namespace Utils;

Cheerio::Cheerio(float x, float y, float z, float radius, Car* car) {

	static MyMesh amesh;
	static bool isMeshCreated = false;

	this->x = x;
	this->y = y;
	this->z = z;

	this->radius = radius;

	this->car = car;

	if (!isMeshCreated) {
		float amb[] = { 0.15f, 0.1f, 0.0f, 1.0f };
		float diff[] = { 0.6f, 0.4f, 0.1f, 1.0f };
		float spec[] = { 0.15f, 0.15f, 0.15f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 60.0f;
		int texIndices[2]{ WOOD_TEX, NO_TEX };
		bool mergeTextureWithColor = true;

		amesh = createTorus(radius / 2, radius, 12, 12);
		setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);
	}
	addPart(amesh);
}

void Cheerio::handleCollision() {
	float mass = 0.01f;
	bool collision = false;

	float angleRad = car->getAngle() * DEG_TO_RAD;

	float collisionMinDistance = car->getColliderSize() / 2 + radius;

	float carToCheerio[3]{ x - car->getX(), 0, z - car->getZ() };
	float carDirection[3]{ cos(angleRad), 0, -sin(angleRad) };
	float dotProductResult = dotProduct(carToCheerio, carDirection);


	if (abs(car->getX() - getX()) <= collisionMinDistance &&
		abs(car->getZ() - getZ()) <= collisionMinDistance &&
		dotProductResult * car->getSpeed() > 0)
		collision = true;

	if (collision) {
		float deltaPos = abs(car->getSpeed()) / mass;
		float deltaPosDir[3]{ carToCheerio[0] + cos(angleRad), 0, carToCheerio[2] - sin(angleRad) };
		normalize(deltaPosDir);

		x += deltaPos * deltaPosDir[0];
		z += deltaPos * deltaPosDir[2];

		car->stop();
	}
}