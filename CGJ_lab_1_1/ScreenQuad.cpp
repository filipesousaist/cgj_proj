#include "ScreenQuad.h"
#include "Utils.h"

using namespace Utils;

float DEFAULT_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };

ScreenQuad::ScreenQuad(float x, float y, float scaleY, float aspectRatio, float colorR, float colorG, float colorB, float colorA) {
	this->x = x;
	this->y = y;
	this->z = 0.5f;
	this->scaleY = 2 * scaleY;
	this->aspectRatio = aspectRatio;

	float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diff[] = { colorR, colorG, colorB, colorA };
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 0.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh amesh = createQuad(1, 1);
	setMeshProperties(&amesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor, true);

	this->addPart(amesh,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f);
}

ScreenQuad::ScreenQuad(float x, float y, float scaleY, float aspectRatio) :
	ScreenQuad(x, y, scaleY, aspectRatio, DEFAULT_COLOR[0], DEFAULT_COLOR[1], DEFAULT_COLOR[2], DEFAULT_COLOR[3]) {
}

void ScreenQuad::resize(float windowWidth, float windowHeight) {
	float windowAspectRatio = windowHeight / windowWidth;

	this->scaleX = this->scaleY * windowAspectRatio / this->aspectRatio;
}