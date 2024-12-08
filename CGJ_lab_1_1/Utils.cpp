#pragma once

#include <string>
#include <iostream>

#include "Utils.h"
#include "constants.h"

using namespace Utils;
using namespace std;

int countTextures(int texIndices[2]) {
	if (texIndices == NULL)
		return 0;
	int count = 0;
	for (int i = 0; i < 2; i++)
		if (texIndices[i] != NO_TEX) {
			count++;
		}
	return count;
}

void Utils::setMeshProperties(MyMesh* mesh,
	float amb[4], float diff[4], float spec[4], float emissive[4], float shininess,
	int texIndices[2], bool mergeTextureWithColor, bool isHUD) {
	memcpy(mesh->mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh->mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh->mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh->mat.emissive, emissive, 4 * sizeof(float));
	mesh->mat.shininess = shininess;
	mesh->mat.texCount = countTextures(texIndices);
	if (mesh->mat.texCount > 0)
		memcpy(mesh->mat.texIndices, texIndices, 2 * sizeof(int));
	mesh->mat.mergeTextureWithColor = mergeTextureWithColor;
	mesh->mat.isHUD = isHUD;
}

void Utils::setMeshProperties(MyMesh* mesh,
	float amb[4], float diff[4], float spec[4], float emissive[4], float shininess,
	int texIndices[2], bool mergeTextureWithColor)
{
	setMeshProperties(mesh, amb, diff, spec, emissive, shininess,
		texIndices, mergeTextureWithColor, false);
}
