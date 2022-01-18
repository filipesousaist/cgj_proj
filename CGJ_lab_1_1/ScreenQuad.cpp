#include "ScreenQuad.h"
#include "Utils.h"

using namespace Utils;


ScreenQuad::ScreenQuad() {
	/*float amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float diff[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	*/
	float amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diff[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 0.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createQuad(1, 1);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh,
		-0.5f, -0.5f, 0.0f,
		1.0f, 1.0f, 1.0f);
}

void ScreenQuad::resize(float scaleX, float scaleY, float windowWidth, float windowHeight) {
	this->scaleX = scaleX;
	this->scaleY = scaleY;

	this->x = (0 - scaleX) * 0.5f;
	this->y = (0 - scaleY) * 0.5f;
}


