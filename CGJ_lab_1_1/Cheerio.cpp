#pragma once

#include "Cheerio.h"
#include <Utils.h>

using namespace std;
using namespace Utils;

Cheerio::Cheerio(float x, float y, float z,
	float sX, float sY, float sZ,
	float angle, float rX, float rY, float rZ) {

	float amb[] = { 0.6f, 0.48f, 0.0f, 1.0f };
	float diff[] = { 0.6f, 0.4f, 0.1f, 1.0f };
	float spec[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 60.0f;
	int texIndices[2]{ WOOD_TEX, NO_TEX };
	bool mergeTextureWithColor = true;

	radius = 0.4f;

	this->x = x;
	this->y = y;
	this->z = z;

	MyMesh amesh = createTorus(0.2f, 0.4f, 12, 12);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(amesh, 0, 0, 0,
		sX, sY, sZ,
		angle, rX, rY, rZ);
}

void Cheerio::addParts() {}