#include "Lives.h"
#include "Utils.h"

using namespace Utils;

float LIFE_SCALE = 1.0f;

Lives::Lives(float x, float y, float scaleY, int num) :
	ScreenQuad(x, y, scaleY, 1.0f / num, 0.0f, 0.0f, 0.0f, 0.0f) {
	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spec[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float shininess = 0.0f;
	int texIndices[] = { LIFE_TEX, NO_TEX };
	bool mergeTextureWithColor = false;
	bool isHUD = true;

	mesh = createQuad(1, 1);
	setMeshProperties(&mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor, isHUD);

	n = 0;
	maxN = num;

	reset();
}

void Lives::loseLife() {
	if (n > 0) {
		--n;
		removeLastPart();
	}
}

void Lives::reset() {
	while (n < maxN) {
		addPart(mesh,
			1.0f / maxN * (n - (maxN - 1) * 0.5f), 0.0f, 0.0f,
			LIFE_SCALE / maxN, LIFE_SCALE, 1.0f);
		++n;
	}
}

bool Lives::areEmpty() {
	return n == 0;
}