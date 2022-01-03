#pragma once
#include "Object.h"
class Orange : public Object
{
private:
	float angle;
	float speed;

	void spawnRandomly(float initialSpeed);	
	void move(int deltaTime);
	void addParts();
public:
	Orange::Orange(float initialSpeed);

	string getType() { return "ORANGE"; }
};

