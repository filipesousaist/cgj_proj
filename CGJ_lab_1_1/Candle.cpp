#pragma once

#include "Candle.h"
#include <Utils.h>

#include <iostream>

using namespace std;
using namespace Utils;


Candle::Candle(float x, float y, float z, float height) {
	static MyMesh amesh;
	static bool isMeshCreated = false;

	if (!isMeshCreated) {
		float amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float diff[] = { 1.0f, 1.0f, 0.7f, 1.0f };
		float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 80.0f;
		int* texIndices = NULL;
		bool mergeTextureWithColor = false;

		amesh = createCylinder(height, 0.75f, 20);
		setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);
		isMeshCreated = true;
	}
	
	addPart(amesh, x, height / 2, z);
}