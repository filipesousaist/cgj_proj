#include "Skybox.h"
#include "Utils.h"

using namespace Utils;

Skybox::Skybox() {
	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createCube();
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, 0, mergeTextureWithColor);

	this->addPart(amesh,0, 0, 0);
}
