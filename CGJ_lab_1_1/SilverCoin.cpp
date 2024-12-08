#include "SilverCoin.h"
#include "Utils.h"

using namespace Utils;

SilverCoin::SilverCoin(float x, float z, Map* map) : Coin(x, z, map)
{
	static MyMesh mesh;
	static bool isMeshCreated = false;

	if (!isMeshCreated) {
		float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
		float diff[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		float spec[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 120.0f;
		int* texIndices = NULL;
		bool mergeTextureWithColor = false;

		mesh = createCylinder(0.2f, RADIUS, 20);
		setMeshProperties(&mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

		isMeshCreated = true;
	}
	addMesh(&mesh);
	setMesh(&mesh);
}

int SilverCoin::getValue()
{
	return 20;
}

string SilverCoin::getType() {
	return "Gold";
}