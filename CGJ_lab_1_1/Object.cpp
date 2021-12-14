#include "Object.h"

Object::Object() {
	this->parts = new vector<Part>;
}

void Object::addPart(MyMesh mesh, float x, float y, float z,
	float sX, float sY, float sZ,
	float angle, float rX, float rY, float rZ) {
	this->parts->push_back(
		{
			mesh,
			{x, y, z},
			{sX, sY, sZ},
			angle,
			{rX, rY, rZ}
		}
	);
}

void Object::addPart(MyMesh mesh, float x, float y, float z,
	float sX, float sY, float sZ) {
	this->addPart(mesh, x, y, z, sX, sY, sZ, 0, 1, 0, 0);
}

void Object::addPart(MyMesh mesh, float x, float y, float z) {
	this->addPart(mesh, x, y, z, 1, 1, 1, 0, 1, 0, 0);
}

void Object::addPart(MyMesh mesh) {
	this->addPart(mesh, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0);
}

void Object::setMeshProperties(MyMesh* mesh,
	float amb[3], float diff[3], float spec[3],
	float emissive[3], float shininess, int texCount)
{
	memcpy(mesh->mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh->mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh->mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh->mat.emissive, emissive, 4 * sizeof(float));
	mesh->mat.shininess = shininess;
	mesh->mat.texCount = texCount;
}

vector<Object::Part>* Object::update(int deltaTime)
{
	this->move(deltaTime);
	return this->parts;
}
