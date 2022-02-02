#include "Firework.h"
#include "Utils.h"

using namespace Utils;

Firework::Firework(float x, float y, float z, GLfloat v, GLfloat phi, GLfloat theta) {

	this->life = 1.0f;
	this->fade = 0.0025f;

	this->x = x;
	this->y = y;
	this->z = z;

	this->vx = v * cos(theta) * sin(phi);
	this->vy = v * cos(phi);
	this->vz = v * sin(theta) * sin(phi);

	this->ax = 0.1f;	//simular um pouco de vento
	this->ay = -0.15f;	//simular a aceleração da gravidade
	this->az = 0.0f;

	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->diff[0] = 0.88f;
	this->diff[1] = 0.55f;
	this->diff[2] = 0.2f;
	this->diff[3] = 1.0f;
	float spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texIndices[] = { PARTICLE_TEX};
	bool mergeTextureWithColor = false;

	MyMesh amesh = createQuad(0.5f, 0.5f);
	setMeshProperties(&amesh, amesh.mat.ambient, amesh.mat.diffuse, amesh.mat.specular, amesh.mat.emissive, amesh.mat.shininess, texIndices, mergeTextureWithColor);

	this->addPart(amesh);
}

void Firework::updateParticle(float h) {
	if (h > 0) {
		this->x += h * this->vx;
		this->y += h * this->vy;
		this->z += h * this->vz;
		this->vx += h * this->ax;
		this->vy += h * this->ay;
		this->vz += h * this->az;
		this->life -= this->fade;
	}
}

void Firework::setDiffuse(float r, float g, float b) {
	diff[0] = r;
	diff[1] = g;
	diff[2] = b;
	diff[3] = this->life;
}

bool Firework::isAlive() {
	return this->life > 0;
}