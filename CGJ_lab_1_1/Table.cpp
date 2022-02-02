#include "Table.h"
#include "Utils.h"

using namespace Utils;

Table::Table() {
	y = -0.1f;

	float amb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diff[] = { 0.99f, 0.99f, 0.99f, 0.30f };
	float spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texIndices[] = { WOOD_TEX, CHECKERS_TEX };
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		-50.0f, -0.1f, -50.0f,
		100.0f, 0.2f, 100.0f);
}
