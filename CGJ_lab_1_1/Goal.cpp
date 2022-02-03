
#include "Utils.h"
#include "Checkpoint.h"

using namespace Utils;

Goal::Goal(float x, float z, float angle, Map* map) : Checkpoint(x, z, angle, map) {
}

void Goal::setReached() {
	if (hasReachedAllCheckpointsExceptGoal())
		Checkpoint::setReached();
}

void Goal::addMesh() {
	float amb[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	float diff[] = { 0.2f, 0.2f, 0.6f, 1.0f };
	float spec[] = { 0.1f, 0.1f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.2f, 1.0f };

	float shininess = 100.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	MyMesh* mesh = getMesh();
	setMeshProperties(mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(*mesh, -CHECKPOINT_OFFSET, 0.0f, -WIDTH * 0.5f,
		1.0f, 0.002f, WIDTH);
}