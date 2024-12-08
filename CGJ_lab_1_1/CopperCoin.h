#pragma once
#include "Coin.h"

class CopperCoin : public Coin
{
public:
	CopperCoin::CopperCoin(float x, float z, Map* map);
	int getValue();
	string getType();
};

