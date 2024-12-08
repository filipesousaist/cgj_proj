#include "Coin.h"
#include "MathUtils.h"

using namespace MathUtils;

Coin::Coin(float x, float z, Map* map) {
	this->x = x;
	this->y = RADIUS + 0.5f;
	this->z = z;
	this->map = map;
	visible = true;
}

void Coin::addMesh(MyMesh* mesh) {
	addPart(*mesh,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		90.0f, 1.0f, 0.0f, 0.0f);
}

void Coin::hide() {
	if (visible) {
		visible = false;
		removeLastPart();
	}
}

void Coin::show() {
	if (!visible) {
		visible = true;
		addMesh(mesh);
	}
}

void Coin::move(int deltaTime) {
	angle = modAngle(angle + ROT_SPEED * deltaTime);
}

void Coin::handleCollision() {
	if (visible) {
		Car* car = map->getCar();

		float collisionMinDistance = car->getColliderSize() / 2 + RADIUS;

		if (abs(car->getX() - x) <= collisionMinDistance &&
			abs(car->getZ() - z) <= collisionMinDistance)
		{
			hide();
			map->collectCoin(this);
		}
	}
}