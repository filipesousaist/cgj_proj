#pragma once
#include "Object.h"
class Candle : public Object
{
private:
	void addParts();
public:
	Candle::Candle(float x, float y, float z,
		float sX, float sY, float sZ,
		float angle, float rX, float rY, float rZ);

	string getType() { return "CANDLE"; }
};

