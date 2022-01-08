#pragma once
#include "Object.h"
class Cheerio : public Object
{
private:
	void addParts();
public:
	Cheerio::Cheerio(float x, float y, float z,
		float sX, float sY, float sZ,
		float angle, float rX, float rY, float rZ);

	string getType() { return "CHEERIO"; }
};


