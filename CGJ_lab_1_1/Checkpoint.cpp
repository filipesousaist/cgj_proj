#include "Utils.h"
#include "Checkpoint.h"

using namespace Utils;

Checkpoint::Checkpoint(float x, float z, float angle, Map* map) {
	this->x = x;
	this->z = z;
	this->angle = angle;
	this->map = map;

	reached = false;
	
	mesh = createCube();	
	addMesh();
}

void Checkpoint::addMesh() {
	float amb[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 0.6f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.1f, 0.1f, 1.0f };
	float emissive[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	
	float amb2[] = { 0.0f, 0.2f, 0.0f, 1.0f };
	float diff2[] = { 0.2f, 0.6f, 0.2f, 1.0f };
	float spec2[] = { 0.1f, 0.2f, 0.1f, 1.0f };
	float emissive2[] = { 0.0f, 0.2f, 0.0f, 1.0f };
	
	float shininess = 100.0f;
	int* texIndices = NULL;
	bool mergeTextureWithColor = false;

	if (reached)
		setMeshProperties(&mesh, amb2, diff2, spec2, emissive2, shininess, texIndices, mergeTextureWithColor);
	else
		setMeshProperties(&mesh, amb, diff, spec, emissive, shininess, texIndices, mergeTextureWithColor);

	addPart(mesh, -CHECKPOINT_OFFSET, 0.0f, -WIDTH * 0.5f,
		1.0f, 0.002f, WIDTH);
}

void Checkpoint::updateMesh() {
	removeLastPart();
	addMesh();
}

void Checkpoint::setReached() {
	if (!(this->reached)) {
		this->reached = true;
		map->getCar()->setSpawnPoint(x, z, angle);
		map->reachCheckpoint();
		updateMesh();
	}
}

void Checkpoint::setNotReached() {
	this->reached = false;
	updateMesh();
}

bool Checkpoint::hasReachedAllCheckpoints() {
	return map->getNumReachedCheckpoints() == map->getNumCheckpoints();
}

bool Checkpoint::hasReachedAllCheckpointsExceptGoal() {
	return map->getNumReachedCheckpoints() == map->getNumCheckpoints() - 1;
}

void Checkpoint::handleCollision() {
	bool flipped = this->angle == 0.0f || this->angle == 180.0f;
	float sizeX = flipped ? 1.0f : WIDTH;
	float sizeZ = flipped ? WIDTH : 1.0f;

	float angleRad = angle * DEG_TO_RAD;

	Car* car = map->getCar();
	if (abs(car->getX() - x) <= (car->getColliderSize() + sizeX) * 0.5f &&
		abs(car->getZ() - z) <= (car->getColliderSize() + sizeZ) * 0.5f)
		setReached();
		
}