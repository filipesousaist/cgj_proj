#include "GoldCoin.h"
#include "Utils.h"

using namespace Utils;

GoldCoin::GoldCoin(float x, float z, Map* map) : Coin(x, z, map)
{
	static MyMesh mesh;
	static bool isMeshCreated = false;

	if (!isMeshCreated) {
		float amb[] = { 0.3f, 0.25f, 0.0f, 1.0f };
		float diff[] = { 1.0f, 0.85f, 0.0f, 1.0f };
		float spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 150.0f;
		int* texIndices = NULL;
		bool mergeTextureWithColor = false;

		mesh = createCylinder(0.2f, RADIUS, 20);
		setMeshProperties(&mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);
		
		isMeshCreated = true;
	}
	addMesh(&mesh);
	setMesh(&mesh);
}

int GoldCoin::getValue()
{
	return 50;
}

string GoldCoin::getType() {
	return "Gold";
}
