#pragma once
#include "Object.h"
class Butter : public Object
{
private:
	void addParts();
public:
	Butter::Butter(float x, float y, float z,
		float sX, float sY, float sZ,
		float angle, float rX, float rY, float rZ);

	float getRadius() {
		return radius;
	};

	string getType() { return "BUTTER"; }

protected:
	float radius;
};

