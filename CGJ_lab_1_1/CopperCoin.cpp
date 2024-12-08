#include "CopperCoin.h"
#include "Utils.h"

using namespace Utils;

CopperCoin::CopperCoin(float x, float z, Map* map) : Coin(x, z, map)
{
	static MyMesh mesh;
	static bool isMeshCreated = false;

	if (!isMeshCreated) {
		float amb[] = { 0.11f, 0.06f, 0.03f, 1.0f };
		float diff[] = { 0.36f, 0.22f, 0.10f, 1.0f };
		float spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float shininess = 100.0f;
		int* texIndices = NULL;
		bool mergeTextureWithColor = false;

		mesh = createCylinder(0.2f, RADIUS, 20);
		setMeshProperties(&mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

		isMeshCreated = true;
	}
	addMesh(&mesh);
	setMesh(&mesh);
}

int CopperCoin::getValue()
{
	return 10;
}

string CopperCoin::getType() {
	return "Gold";
}