#include "Pawn.h"
#include "Utils.h"

using namespace Utils;

Pawn::Pawn(float x, float z) {
	this->x = x;
	this->z = z;

	float amb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diff[] = { 0.6f, 0.6f, 0.8f, 0.4f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 0.4f };
	float shininess = 50.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createPawn();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		0.0f, 0.0f, 0.0f,
		3.0f, 3.0f, 3.0f);
}
