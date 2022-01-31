//
// CGJ: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: Jo�o Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "Texture_Loader.h"
#include "avtFreeType.h"

#include "Object.h"
#include "Butter.h"
#include "Candle.h"
#include "Car.h"
#include "Cheerio.h"
#include "Orange.h"
#include "Pawn.h"
#include "ScreenQuad.h"
#include "Table.h"
#include "Tree.h"
#include "Firework.h"
#include "constants.h"
#include "l3DBillboard.h"
#include "Utils.h"
#include <flare.h>
#include <Skybox.h>

#define frand()		((float)rand()/RAND_MAX)

using namespace std;
using namespace Utils;

constexpr char CAPTION[] = "Lab 1.1";
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

unsigned int lastTime = 0; // Time of last frame in milliseconds

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

//File with the font
constexpr char FONT_NAME[] = "fonts/arial.ttf";

vector<Object*> gameObjects;
vector<Firework*> fireworks;
int num_dead_particles = 0;

Car* car;

Camera cameraProjection;

float camRatio;

ScreenQuad* pauseQuad;
MyMesh flareQuad;

Skybox* skybox;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint model_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint tex_loc[2];
GLint texMode_uniformId;
GLint tex_normalMap_loc;
GLint tex_skyBoxMap_loc;


GLuint FlareTextureArray[5];
GLuint TextureArray[8];

int windowWidth = 0;
int windowHeight = 0;

//Flare effect
FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates

// Camera Position
float camX, camY, camZ;
float camWorld[4];

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 10.0f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];

float directionalLightPos[4] { 1.0f, 1000.0f, 1.0f, 0.0f };

float pointLightPos[NUM_POINT_LIGHTS][4] {
	{-35.0f, 4.0f, -35.0f, 1.0f},
	{-35.0f, 4.0f, 35.0f, 1.0f},
	{35.0f, 4.0f, -35.0f, 1.0f},
	{35.0f, 4.0f, 35.0f, 1.0f},
	{0.0f, 4.0f, -15.0f, 1.0f},
	{0.0f, 4.0f, 15.0f, 1.0f}
};

bool day = true;
bool dayKey = false;

bool candles = true;
bool candlesKey = false;

bool headlights = true;
bool headlightsKey = false;

bool fog = false;
bool fogKey = false;

bool showText = true;
bool showTextKey = false;

bool paused = false;
bool pausedKey = false;

bool flare = false;
bool flareKey = false;

bool bumpmap = false;
bool bumpmapKey = false;

bool firework = false;
bool fireworkKey = false;

inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}
void initFireworks()
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		fireworks.push_back(new Firework(0, 7.5f, 0,
			0.8f * frand() + 0.2f, frand() * PI, 2.0f * frand() * PI));
	}
}

void renderFirework(Firework* particle) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vector<Object::Part>* parts = particle->getParts();
	particle->updateParticle(0.033f);

	for (const Object::Part& part : *parts) {
		GLint loc;

		glActiveTexture(GL_TEXTURES[0]);
		glBindTexture(GL_TEXTURE_2D, TextureArray[PARTICLE_TEX]);
		glUniform1i(tex_loc[0], 0);

		loc = glGetUniformLocation(shader.getProgramIndex(), "mergeTextureWithColor");
		glUniform1i(loc, part.mesh.mat.mergeTextureWithColor);

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, -1);

		glDepthMask(GL_FALSE);  //Depth Buffer Read Only

		if (particle->isAlive()) {
			particle->setDiffuse(0.882, 0.552, 0.211);


			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, particle->getDiffuse());

			float worldPos[3]{ particle->getX(), particle->getY(), particle->getZ() };

			pushMatrix(MODEL);
			translate(MODEL, particle->getX(), particle->getY(), particle->getZ());
			if (cameraProjection == Camera::PERSPECTIVE)
				l3dBillboardSphericalBegin(camWorld, worldPos);
			else if (cameraProjection == Camera::CAR)
				l3dBillboardCylindricalBegin(camWorld, worldPos);

			pushMatrix(MODEL);
			translate(MODEL, particle->getX(), particle->getY(), particle->getZ());

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			// Render mesh
			glBindVertexArray(part.mesh.vao);
			glDrawElements(part.mesh.type, part.mesh.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			popMatrix(MODEL);
			popMatrix(MODEL);


		}
		else num_dead_particles++;
	}
}

void refresh(int value)
{
	glutTimerFunc(1000 / 60, refresh, 0);
	glutPostRedisplay();
}

void setCameraProjection() {
	loadIdentity(PROJECTION);

	switch (cameraProjection)
	{
	case Camera::ORTHOGONAL:
		ortho(-50 * camRatio, 50 * camRatio, -50, 50, 1, 1500);
		break;
	case Camera::PERSPECTIVE:
	case Camera::CAR:
		perspective(53.13f, camRatio, 0.1f, 1000.0f);
		break;
	default:
		break;
	}
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	windowWidth = w;
	windowHeight = h;

	// Update pause quad size and position
	float scale = fminf(windowWidth, windowHeight);
	pauseQuad->resize(scale * 1.0f, scale * 0.2f, w, h);

	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix

	camRatio = (1.0f * w) / h;

	setCameraProjection();
}

void updateCarCamera() {
	float x = car->getX();
	float y = car->getY();
	float z = car->getZ();

	
	float camLocal[4]{ camX, camY, camZ, 1 };

	pushMatrix(MODEL);
	translate(MODEL, x, y, z);
	rotate(MODEL, car->getAngle(), 0, 1, 0);
	multMatrixPoint(MODEL, camLocal, camWorld);
	lookAt(
		camWorld[0], camWorld[1], camWorld[2],
		x, y, z,
		0, 1, 0
	);
	popMatrix(MODEL);
}


// ------------------------------------------------------------
//
// Render stufff
//

void setCameraLookAt() {
	// set the camera using a function similar to gluLookAt
	switch (cameraProjection) {
	case Camera::ORTHOGONAL:
		lookAt(0, 100, 0, 0, 0, 0, 1, 0, 0); break;
	case Camera::PERSPECTIVE:
		lookAt(-66, 30, -66, 0, 0, 0, 0, 1, 0); 
		camWorld[0] = -66;
		camWorld[1] = 30;
		camWorld[2] = -66;
		break;
	case Camera::CAR:
		updateCarCamera(); break;
	}
}

void renderLights() {
	GLint loc;

	loc = glGetUniformLocation(shader.getProgramIndex(), "day");
	glUniform1i(loc, day);

	loc = glGetUniformLocation(shader.getProgramIndex(), "candles");
	glUniform1i(loc, candles);

	loc = glGetUniformLocation(shader.getProgramIndex(), "headlights");
	glUniform1i(loc, headlights);

	loc = glGetUniformLocation(shader.getProgramIndex(), "fog");
	glUniform1i(loc, fog);

	float res[4];
	multMatrixPoint(VIEW, directionalLightPos, res);
	GLint lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "directionalLightPos");
	glUniform3fv(lPos_uniformId, 1, res);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		float res[4];
		multMatrixPoint(VIEW, pointLightPos[i], res);   //lightPos definido em World Coord so is converted to eye space
		stringstream ss;
		ss.str("");
		ss << "pointLightPos[" << i << "]";
		GLint plPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), ss.str().c_str());
		glUniform3fv(plPos_uniformId, 1, res);
	}
}

void renderObject(Object* obj) {
	vector<Object::Part>* parts = obj->getParts();

	for (const Object::Part& part : *parts) {
		GLint loc;
		// textures
		for (int t = 0; t < part.mesh.mat.texCount; t++) {
			glUniform1i(texMode_uniformId, 0);
			glActiveTexture(GL_TEXTURES[t]);
			glBindTexture(GL_TEXTURE_2D, TextureArray[part.mesh.mat.texIndices[t]]);
			glUniform1i(tex_loc[t], t);
		}
		if (part.mesh.mat.texIndices[0] == ORANGE_TEX) { //check if it is orange
			if (bumpmap) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, TextureArray[ORANGE_Norm]); //normal.tga
				glUniform1i(texMode_uniformId, 2);
				glUniform1i(tex_normalMap_loc, 1);
			}
		}
		loc = glGetUniformLocation(shader.getProgramIndex(), "mergeTextureWithColor");
		glUniform1i(loc, part.mesh.mat.mergeTextureWithColor);

		if (part.mesh.mat.texIndices[0] == TREE_TEX) {
			float worldPos[3]{ part.position[0], part.position[1], part.position[2] };

			pushMatrix(MODEL);
			translate(MODEL, part.position[0], part.position[1], part.position[2]);
			if (cameraProjection == Camera::PERSPECTIVE)
				l3dBillboardSphericalBegin(camWorld, worldPos);
			else if (cameraProjection == Camera::CAR)
				l3dBillboardCylindricalBegin(camWorld, worldPos);

		}

		// send the material
		if (part.mesh.mat.texIndices[0] != TREE_TEX) {
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, part.mesh.mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, part.mesh.mat.diffuse);
		}
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, part.mesh.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, part.mesh.mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, part.mesh.mat.texCount);
		pushMatrix(MODEL);

		if (part.mesh.mat.texIndices[0] != TREE_TEX) {
			translate(MODEL, obj->getX(), obj->getY(), obj->getZ());
			rotate(MODEL, obj->getAngle(), 0, 1, 0);
			rotate(MODEL, obj->getRollAngle(), 0, 0, -1);
			scale(MODEL, obj->getScaleX(), obj->getScaleY(), obj->getScaleZ());
			translate(MODEL, part.position[0], part.position[1], part.position[2]);
			rotate(MODEL, part.angle, part.rotationAxis[0], part.rotationAxis[1], part.rotationAxis[2]);
			scale(MODEL, part.scale[0], part.scale[1], part.scale[2]);
		}
		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(part.mesh.vao);

		if (!shader.isProgramValid()) {
			std::printf("Program Not Valid!\n");
			exit(1);
		}
		glDrawElements(part.mesh.type, part.mesh.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);

		if (part.mesh.mat.texIndices[0] == TREE_TEX)
			popMatrix(MODEL);
	}
}

void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {
	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0

	//glUniform1i(texMode_uniformId, 3); // draw modulated textured particles 
	glUniform1i(tex_loc[0], 0);  //use TU 0

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
			glUniform1i(loc, -1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(flareQuad.vao);
			glDrawElements(flareQuad.type, flareQuad.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
}

void renderHUDShapes() {
	pushMatrix(MODEL);
	pushMatrix(VIEW);
	pushMatrix(PROJECTION);

	loadIdentity(MODEL);
	loadIdentity(VIEW);
	loadIdentity(PROJECTION);

	pushMatrix(VIEW);
	loadIdentity(VIEW); //viewer at World origin, looking down at negative z direction
	ortho(-windowWidth * 0.5f, windowWidth * 0.5f, -windowHeight * 0.5f, windowHeight * 0.5f, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//renderObject(pauseQuad);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	popMatrix(MODEL);
	popMatrix(VIEW);
	popMatrix(PROJECTION);
}

void renderText() {
	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	pushMatrix(MODEL);
	pushMatrix(VIEW);
	pushMatrix(PROJECTION);
	
	loadIdentity(MODEL);
	loadIdentity(VIEW);
	loadIdentity(PROJECTION);

	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	RenderText(shaderText, "X " + std::to_string(car->getX()) + " m", 25.0f, 125.0f, 0.5f, 0.5f, 0.5f, 0.8f);
	RenderText(shaderText, "Z " + std::to_string(car->getZ()) + " m", 25.0f, 100.0f, 0.5f, 0.5f, 0.5f, 0.8f);
	RenderText(shaderText, "Angle " + std::to_string(car->getAngle()) + " deg", 25.0f, 75.0f, 0.5f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "Speed " + std::to_string(car->getSpeed()), 25.0f, 50.0f, 0.5f, 0.5f, 0.2f, 0.8f);
	RenderText(shaderText, "Angular speed " + std::to_string(car->getAngularSpeed()), 25.0f, 25.0f, 0.5f, 0.5f, 0.8f, 0.2f);
	
	if (paused) {
		float HUDscale = fminf(windowWidth, windowHeight);
		
		RenderText(shaderText, "GAME PAUSED", windowWidth * 0.243f, windowHeight * 0.465f + 0.25f,
			0.002f * HUDscale, 1.0f, 1.0f, 1.0f);
	}

	popMatrix(MODEL);
	popMatrix(VIEW);
	popMatrix(PROJECTION);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void renderSkybox() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[SKY_TEX]);
	glUniform1i(tex_skyBoxMap_loc, 0);

	glUniform1i(texMode_uniformId, 3);

	//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CW); // set clockwise vertex order to mean the front

	pushMatrix(MODEL);
	pushMatrix(VIEW);  //se quiser anular a translação

	//  Fica mais realista se não anular a translação da câmara 
	// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
	mMatrix[VIEW][12] = 0.0f;
	mMatrix[VIEW][13] = 0.0f;
	mMatrix[VIEW][14] = 0.0f;

	scale(MODEL, 100.0f, 100.0f, 100.0f);
	translate(MODEL, -0.5f, -0.5f, -0.5f);

	// send matrices to OGL
	glUniformMatrix4fv(model_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //Transformação de modelação do cubo unitário para o "Big Cube"
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);

	vector<Object::Part>* parts = skybox->getParts();

	for (const Object::Part& part : *parts) {
		glBindVertexArray(part.mesh.vao);
		glDrawElements(part.mesh.type, part.mesh.numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		popMatrix(MODEL);
		popMatrix(VIEW);
	}
	glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void renderScene(void) {

	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = paused ? 0 : currentTime - lastTime;

	FrameCount++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//The glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	
	setCameraLookAt();
	
	// use our shader
	glUseProgram(shader.getProgramIndex());

	renderSkybox();

	renderLights();

	for (Object* obj : gameObjects)
		obj->update(deltaTime);

	for (Object* obj : gameObjects)
		renderObject(obj);

	for (Object* obj : gameObjects)
		obj->handleCollision();

	if (firework) {
		for (Firework* particle : fireworks)
			renderFirework(particle);

		glDepthMask(GL_TRUE);  //Depth Buffer Read Only
		if (num_dead_particles == MAX_PARTICLES) {
			firework = false;
			num_dead_particles = 0;
			fireworks.clear();
			printf("All particles dead\n");
		}

	}

	if (flare && candles) {
		int flarePos[2];
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		pushMatrix(MODEL);
		loadIdentity(MODEL);
		computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

		if (!project(pointLightPos[0], lightScreenPos, m_viewport))
			printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
		flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
		flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
		popMatrix(MODEL);

		//viewer looking down at  negative z direction
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		pushMatrix(VIEW);
		loadIdentity(VIEW);
		ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
	}

	if (showText) {
		renderHUDShapes();
		renderText();
	}
	
	glutSwapBuffers();

	lastTime = currentTime;
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {
	case 27:
		glutLeaveMainLoop();
		break;

	case 'c': 
		std::printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'n': glDisable(GL_MULTISAMPLE); break;

	case '1':
		cameraProjection = Camera::ORTHOGONAL; setCameraProjection(); break;
	case '2':
		cameraProjection = Camera::PERSPECTIVE; setCameraProjection(); break;
	case '3':
		cameraProjection = Camera::CAR; setCameraProjection(); break;

	case 'w':
		car->accelerate(true); break;
	case 's':
		car->accelerateBack(true); break;
	case 'a':
		car->turnLeft(true); break;
	case 'd':
		car->turnRight(true); break;
	case 'o': // night mode
		if (!dayKey) {
			dayKey = true;
			day = !day;
		}
		break;
	case 'p': // candles
		if (!candlesKey) {
			candlesKey = true;
			candles = !candles;
		}
		break;
	case 'h': // headlights
		if (!headlightsKey) {
			headlightsKey = true;
			headlights = !headlights;
		}
		break;
	case 'f': // fog
		if (!fogKey) {
			fogKey = true;
			fog = !fog;
		}
		break;
	case 't': // text
		if (!showTextKey) {
			showTextKey = true;
			showText = !showText;
		}
		break;

	case ' ': // pause
		if (!pausedKey) {
			pausedKey = true;
			paused = !paused;
		}
		break;
	
	case 'k': //fireworks
		if (!fireworkKey) {
			if (firework) {
				firework = false;
				fireworks.clear();
			}
			else {
				initFireworks();
				fireworkKey = true;
				firework = true;
			}
		}
		break;
	
	case 'g': // flare
		if (candlesKey) flare = false;
		else
			if (flare) {
				flare = false;

			}
			else flare = true;
		break;

	case 'b': // bumpmap
		if (bumpmap) bumpmap = false;
		else bumpmap = true;
		break;
	}
}

void processKeysUp(unsigned char key, int xx, int yy)
{
	switch (key) {
	case 'w':
		car->accelerate(false); break;
	case 's':
		car->accelerateBack(false); break;
	case 'a':
		car->turnLeft(false); break;
	case 'd':
		car->turnRight(false); break;
	case 'o':
		dayKey = false; break;
	case 'p':
		candlesKey = false; break;
	case 'h':
		headlightsKey = false; break;
	case 'f':
		fogKey = false; break;
	case 't':
		showTextKey = false; break;
	case ' ':
		pausedKey = false; break;
	case 'g':
		flareKey = false; break;
	case 'k':
		fireworkKey = false; break;
	}
}

// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	if (cameraProjection == Camera::CAR) {
		// start tracking the mouse
		if (state == GLUT_DOWN) {
			startX = xx;
			startY = yy;
			if (button == GLUT_LEFT_BUTTON)
				tracking = 1;
			else if (button == GLUT_RIGHT_BUTTON)
				tracking = 2;
		}

		//stop tracking the mouse
		else if (state == GLUT_UP) {
			if (tracking == 1) {
				alpha = fmod(alpha - (xx - startX), 360);
				beta = fmod(beta + yy - startY, 360);
			}
			else if (tracking == 2) {
				r += (yy - startY) * 0.01f;
				if (r < 0.1f)
					r = 0.1f;
			}
			tracking = 0;
		}
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{
	if (cameraProjection == Camera::CAR) {
		int deltaX, deltaY;
		float alphaAux, betaAux;
		float rAux;

		deltaX = -xx + startX;
		deltaY = yy - startY;

		// left mouse button: move camera
		if (tracking == 1) {


			alphaAux = alpha + deltaX;
			betaAux = beta + deltaY;

			if (betaAux > 85.0f)
				betaAux = 85.0f;
			else if (betaAux < -85.0f)
				betaAux = -85.0f;
			rAux = r;
		}
		// right mouse button: zoom
		else if (tracking == 2) {

			alphaAux = alpha;
			betaAux = beta;
			rAux = r + (deltaY * 0.01f);
			if (rAux < 0.1f)
				rAux = 0.1f;
		}

		if (tracking == 1 || tracking == 2) {
			camX = rAux * sin(alphaAux * DEG_TO_RAD) * cos(betaAux * DEG_TO_RAD);
			camZ = rAux * cos(alphaAux * DEG_TO_RAD) * cos(betaAux * DEG_TO_RAD);
			camY = rAux * sin(betaAux * DEG_TO_RAD);
		}
	}

	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {
	if (cameraProjection == Camera::CAR) {
		r += direction * 0.1f;
		if (r < 0.1f)
			r = 0.1f;

		camX = r * sin(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
		camZ = r * cos(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
		camY = r *   						     sin(beta * DEG_TO_RAD);
	}

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//

GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TANGENT_ATTRIB, "tangent");

	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	model_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_Model");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	tex_loc[0] = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc[1] = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	tex_skyBoxMap_loc = glGetUniformLocation(shader.getProgramIndex(), "skyBoxMap");


	std::printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	std::printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());
	
	return shader.isProgramLinked() && shaderText.isProgramLinked();
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void createScene() {
	//Texture Object definition

	glGenTextures(8, TextureArray);
	Texture2D_Loader(TextureArray, "img/stone.tga", STONE_TEX);
	Texture2D_Loader(TextureArray, "img/lightwood.tga", WOOD_TEX);
	Texture2D_Loader(TextureArray, "img/checker.png", CHECKERS_TEX);
	Texture2D_Loader(TextureArray, "img/orangeTex.png", ORANGE_TEX);
	Texture2D_Loader(TextureArray, "img/Orange_001_NORM.jpg", ORANGE_Norm);
	Texture2D_Loader(TextureArray, "img/tree.tga", TREE_TEX);
	Texture2D_Loader(TextureArray, "img/particle.tga", PARTICLE_TEX);

	//Sky Box Texture Object
	const char* filenames[] = { "img/posx.jpg", "img/negx.jpg", "img/posy.jpg", "img/negy.jpg", "img/posz.jpg", "img/negz.jpg" };

	TextureCubeMap_Loader(TextureArray, filenames, SKY_TEX);

	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "img/crcl.tga", 0);
	Texture2D_Loader(FlareTextureArray, "img/flar.tga", 1);
	Texture2D_Loader(FlareTextureArray, "img/hxgn.tga", 2);
	Texture2D_Loader(FlareTextureArray, "img/ring.tga", 3);
	Texture2D_Loader(FlareTextureArray, "img/sun.tga", 4);

	//Create skybox
	skybox = new Skybox();

	//createTable();
	gameObjects.push_back(new Table());
	
	car = new Car(&shader, 2.0f, 1.0f);
	gameObjects.push_back(car);

	for (int o = 0; o < NUM_ORANGES; o++)
		gameObjects.push_back(new Orange(car));

	float butterPositions[]{
		14, 1,
		19, -0.75f,
		28, 1
	};

	float butterSizes[]{
		2.0f, 1.0f,
		2.0f, 1.0f,
		1.0f, 2.0f
	};
	for (int i = 0; i < sizeof(butterPositions) / (sizeof(float) * 2); i++)
	{
		Butter* b = new Butter(
			butterPositions[2 * i], 0, butterPositions[2 * i + 1],
			butterSizes[2 * i], butterSizes[2 * i + 1], car);
		gameObjects.push_back(b);
	}

	float signs[]{ -1, 1 };

	for (int sign = 0; sign < 2; sign++) {
		float zSign = signs[sign];

		for (float x = -40.0f; x <= 40.0f; x += 2) {
			Cheerio* c = new Cheerio(x, 0.1f, 3.0f * zSign, 0.4f, car);
			gameObjects.push_back(c);
		}
	}


	float candlePositions[] {
		-35.0f, -35.0f,
		-35.0f, 35.0f,
		35.0f, -35.0f,
		35.0f, 35.0f,
		0.0f, -15.0f,
		0.0, 15.0f
	};

	for (int i = 0; i < sizeof(candlePositions) / (2 * sizeof(float)); i++)
		gameObjects.push_back(new Candle(
			candlePositions[2 * i], 0, candlePositions[2 * i + 1], 3.5f));

	gameObjects.push_back(new Tree(0, 2.5f, 25));
	gameObjects.push_back(new Tree(0, 2.5f, -25));

	gameObjects.push_back(new Pawn());

	gameObjects.push_back(new ScreenQuad());

	pauseQuad = new ScreenQuad();

	// create geometry and VAO of the quad for flare elements
	flareQuad = createQuad(1, 1);
	flareQuad.mat.texCount = 1;

	//Load flare from file
	loadFlareFile(&AVTflare, "flare.txt");
}

void init()
{
	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		std::printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(FONT_NAME);

	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
	camZ = r * cos(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
	camY = r *   						 sin(beta * DEG_TO_RAD);

	cameraProjection = Camera::ORTHOGONAL;

	createScene();
	
	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);		 // cull back face
	glFrontFace(GL_CCW); // set counter-clockwise vertex order to mean the front
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}



// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);   // Use it to count number of frames rendered per second
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(0, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processKeysUp);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	std::printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	std::printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	std::printf ("Version: %s\n", glGetString (GL_VERSION));
	std::printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}

unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}
void    loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}



