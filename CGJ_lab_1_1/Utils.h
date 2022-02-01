#pragma once

#include <geometry.h>

#include "constants.h"

namespace Utils
{
	void setMeshProperties(MyMesh* mesh,
		float amb[4], float diff[4], float spec[4], float emissive[4], float shininess,
		int texIndices[2], bool mergeTextureWithColor, bool isHUD);

	void setMeshProperties(MyMesh* mesh,
		float amb[4], float diff[4], float spec[4], float emissive[4], float shininess,
		int texIndices[2], bool mergeTextureWithColor);
};

