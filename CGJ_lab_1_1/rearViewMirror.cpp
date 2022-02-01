#include "RearViewMirror.h"
#include "Utils.h"

using namespace Utils;

RearViewMirror::RearViewMirror() {
	float amb[] = { 0.3f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 0.8f, 0.1f, 0.1f, 0.0f };
	float spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		-0.25f, 0.5f, -0,
		0.5f, 0.3f, 1.0f);
}
