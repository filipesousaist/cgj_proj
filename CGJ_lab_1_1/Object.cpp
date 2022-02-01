#pragma once
#include "Object.h"
#include <AVTmathLib.h>

Object::Object() {
	x = y = z = 0;
	angle = rollAngle = 0;
	scaleX = scaleY = scaleZ = 1.0f;
	parts = new vector<Part>;
}

void Object::addPart(MyMesh mesh, float x, float y, float z,
	float sX, float sY, float sZ,
	float angle, float rX, float rY, float rZ) {
	parts->push_back(
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
	addPart(mesh, x, y, z, sX, sY, sZ, 0, 1, 0, 0);
}

void Object::addPart(MyMesh mesh, float x, float y, float z) {
	addPart(mesh, x, y, z, 1, 1, 1, 0, 1, 0, 0);
}

void Object::addPart(MyMesh mesh) {
	addPart(mesh, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0);
}

void Object::removeLastPart() {
	parts->pop_back();
}

void Object::update(int deltaTime)
{
	move(deltaTime);
}

vector<Object::Part>* Object::getParts() {
	return parts;
}

