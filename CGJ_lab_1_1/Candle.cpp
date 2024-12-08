#pragma once

#include "Candle.h"
#include <Utils.h>

#include <iostream>

using namespace std;
using namespace Utils;


Candle::Candle(float x, float y, float z, float height) {
	static MyMesh baseMesh;
	static MyMesh topMesh;
	static bool isMeshCreated = false;

	if (!isMeshCreated) {
		float amb[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float diff[] = { 1.0f, 1.0f, 0.7f, 1.0f };
		float spec[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 60.0f;
		int* texIndices = NULL;
		bool mergeTextureWithColor = false;
		
		baseMesh = createCylinder(height, height * 0.2f, 20);
		setMeshProperties(&baseMesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

		float amb2[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		float diff2[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float spec2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float emissive2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess2 = 30.0f;
		int* texIndices2 = NULL;
		bool mergeTextureWithColor2 = false;

		topMesh = createCylinder(height * 0.2f, height * 0.02f, 14);
		setMeshProperties(&baseMesh, amb2, diff2, spec2, emissive2, shininess2, texIndices2, mergeTextureWithColor2);
	}
	
	addPart(baseMesh, x, height * 0.5f, z);
	addPart(topMesh, x, height * 1.1f, z);
}