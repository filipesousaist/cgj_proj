#pragma once

#include "Butter.h"
#include <Utils.h>

using namespace std;
using namespace Utils;

Butter::Butter(float x, float y, float z,
	float sX, float sY, float sZ,
	float angle, float rX, float rY, float rZ) {
	
	MyMesh amesh;

	float amb[] = { 0.6f, 0.48f, 0.0f, 1.0f };
	float diff[] = { 0.8f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;
	radius = 2.0f;

	this->x = x;
	this->y = y;
	this->z = z;

	amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(amesh, 0, 0, 0,
		sX, sY, sZ,
		angle, rX, rY, rZ);
}

void Butter::addParts() {}