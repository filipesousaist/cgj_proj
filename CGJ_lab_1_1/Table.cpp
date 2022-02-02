#include "Table.h"
#include "Utils.h"

using namespace Utils;

Table::Table(int width, int height) {
	y = -0.1f;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.6f, 0.4f, 0.2f, 1.0f };
	float spec[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texIndices[] = { WOOD_TEX, CHECKERS_TEX };
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		-width * 0.5f, -0.1f, -height * 0.5f,
		width, 0.2f, height);
}
