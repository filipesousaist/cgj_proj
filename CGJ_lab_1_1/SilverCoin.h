#pragma once
#include "Coin.h"

class SilverCoin : public Coin
{
public:
	SilverCoin::SilverCoin(float x, float z, Map* map);
	int getValue();
	string getType();
};

