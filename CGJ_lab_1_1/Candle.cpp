#pragma once

#include "Candle.h"
#include <Utils.h>

using namespace std;
using namespace Utils;

Candle::Candle(float x, float y, float z,
	float sX, float sY, float sZ,
	float angle, float rX, float rY, float rZ) {

	float amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 0.7f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;
	float height = 3.5f;

	MyMesh amesh = createCylinder(height, 0.75f, 20);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(amesh, x, height / 2, z,
		sX, sY, sZ,
		angle, rX, rY, rZ);
}

void Candle::addParts() {}