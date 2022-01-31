#pragma once
#include "Object.h"
class Firework : public Object
{
private:
	float life;
	float fade;
	GLfloat vx, vy, vz;  //velocidade
	GLfloat ax, ay, az;  //aceleracao

	float diff[4];

public:
	Firework::Firework(float x, float y, float z, GLfloat v, GLfloat phi, GLfloat theta);

	void updateParticle(float h);

	void setDiffuse(float r, float g, float b);

	bool isAlive();

	float* getDiffuse() {
		return diff;
	}

	float getX() {
		return x;
	}
	float getY() {
		return y;
	}
	float getZ() {
		return z;
	}
};

