#include "Tree.h"
#include "Utils.h"

using namespace Utils;

Tree::Tree(float x, float y, float z) {
	float amb[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	float diff[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 10.0f;
	int texIndices[] = { TREE_TEX, NO_TEX };
	bool mergeTextureWithColor = false;

	MyMesh amesh = createQuad(5, 5);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		x, y, z);
}
