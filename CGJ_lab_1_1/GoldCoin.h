#pragma once
#include "Coin.h"

class GoldCoin : public Coin
{
public:
	GoldCoin::GoldCoin(float x, float z, Map* map);
	int getValue();
	string getType();
};

