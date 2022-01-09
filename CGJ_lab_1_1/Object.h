#pragma once

#include "geometry.h"
#include <vector>
#include <string>

using namespace std;

class Object
{
public:
	Object();

	struct Part {
		MyMesh mesh;
		float position[3];
		float scale[3];
		float angle;
		float rotationAxis[3];
	};
	
	void update(int deltaTime);

	vector<Part>* getParts();

	float getX() {
		return x;
	};
	float getY() {
		return y;
	};
	float getZ() {
		return z;
	};
	float getAngle() {
		return angle;
	}
	float getRollAngle() {
		return rollAngle;
	}
	void getRollAxis(float* rollAxis); // By reference

	virtual void handleCollision() {};

protected:
	float x;
	float y;
	float z;
	float angle;
	float rollAngle;

	void addPart(MyMesh mesh, float x, float y, float z, 
		float sX, float sY, float sZ, 
		float angle, float rX, float rY, float rZ);
	void addPart(MyMesh mesh, float x, float y, float z,
		float sX, float sY, float sZ);
	void addPart(MyMesh mesh, float x, float y, float z);
	void addPart(MyMesh mesh);

	virtual void move(int deltaTime) {};

private:
	vector<Part>* parts;
	
};

