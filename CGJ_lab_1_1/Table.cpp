#include "Table.h"
#include "Utils.h"

using namespace Utils;

Table::Table(int width, int height) {
	y = -0.1f;

	this->width = width;
	this->height = height;

	float amb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diff[] = { 0.99f, 0.99f, 0.99f, 0.30f };
	float spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texIndices[] = { WOOD_TEX, CHECKERS_TEX };
	bool mergeTextureWithColor = false;

	translucentMesh = createCube();
	setMeshProperties(&translucentMesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	float diff2[] = { 0.99f, 0.99f, 0.99f, 1.0f };
	opaqueMesh = createCube();
	setMeshProperties(&opaqueMesh, amb, diff2, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addMesh(opaqueMesh);
}

void Table::addMesh(MyMesh mesh) {
	addPart(mesh,
		-width * 0.5f, -0.1f, -height * 0.5f,
		width, 0.2f, height);
}

void Table::setTranslucent(bool translucent) {
	removeLastPart();
	addMesh(translucent ? translucentMesh : opaqueMesh);
}
