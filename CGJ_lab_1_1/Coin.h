#pragma once

#include <geometry.h>

#include "Object.h"
#include "Map.h"

class Map;

const float RADIUS = 0.5f;
const float ROT_SPEED = 3e-2f;

class Coin : public Object
{
private:
	Map* map;
	MyMesh* mesh;
	bool visible;
protected:
	void addMesh(MyMesh* mesh);
	void setMesh(MyMesh* mesh) { this->mesh = mesh; }
public:
	Coin::Coin(float x, float z, Map* map);
	virtual int getValue() = 0;
	virtual string getType() = 0;
	void hide();
	void show();
	void move(int deltaTime);
	void handleCollision();
};

