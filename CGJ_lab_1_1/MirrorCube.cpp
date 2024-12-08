#include "MirrorCube.h"
#include "Utils.h"

using namespace Utils;

MirrorCube::MirrorCube(float x, float z) {
	this->x = x;
	this->z = z;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texIndices[2] { SUGAR_TEX, NO_TEX };
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh, 0.0f, 0.0f, 0.0f,
		2.0f, 2.0f, 2.0f);
}