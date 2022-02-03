#pragma once

#include <geometry.h>

#include "Object.h"

class Table : public Object
{
private:
	int width;
	int height;
	MyMesh translucentMesh;
	MyMesh opaqueMesh;
	void addMesh(MyMesh mesh);
public:
	Table::Table(int width, int height);
	void setTranslucent(bool translucent);
};

