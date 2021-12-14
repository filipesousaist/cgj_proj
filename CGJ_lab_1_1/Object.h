#pragma once

#include "geometry.h"
#include <vector>

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

	static void setMeshProperties(MyMesh* mesh,
		float amb[3], float diff[3], float spec[3],
		float emissive[3], float shininess, int texCount);
	virtual void addParts() = 0;
	virtual void move(int deltaTime) {};
	vector<Part>* update(int deltaTime);

protected:
	void addPart(MyMesh mesh, float x, float y, float z, 
		float sX, float sY, float sZ, 
		float angle, float rX, float rY, float rZ);
	void addPart(MyMesh mesh, float x, float y, float z,
		float sX, float sY, float sZ);
	void addPart(MyMesh mesh, float x, float y, float z);
	void addPart(MyMesh mesh);

private:
	vector<Part>* parts;
	
};

