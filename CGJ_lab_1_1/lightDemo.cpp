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
#include "Lives.h"
#include "Orange.h"
#include "Pawn.h"
#include "ScreenQuad.h"
#include "Table.h"
#include "constants.h"
#include "Utils.h"

using namespace std;
using namespace Utils;

constexpr char CAPTION[] = "Micro Machines";
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

Car* car;

Camera cameraProjection;

float camRatio;

ScreenQuad* pauseQuad;

ScreenQuad* gameOverQuad;
ScreenQuad* restartQuad;

const int NUM_LIVES = 5;
Lives* lives;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint tex_loc[2];
GLint texMode_uniformId;

const int NUM_TEXTURES = 5;
GLuint TextureArray[NUM_TEXTURES];

int windowWidth = 0;
int windowHeight = 0;

// Camera Position
float camX, camY, camZ;

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

bool restartKey = false;

bool gameOver = false;

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
	pauseQuad->resize(w, h);
	gameOverQuad->resize(w, h);
	restartQuad->resize(w, h);
	lives->resize(w, h);

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

	float camWorld[4];
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

void setCameraLookAt() {
	// set the camera using a function similar to gluLookAt
	switch (cameraProjection) {
	case Camera::ORTHOGONAL:
		lookAt(0, 100, 0, 0, 0, 0, 1, 0, 0); break;
	case Camera::PERSPECTIVE:
		lookAt(-66, 30, -66, 0, 0, 0, 0, 1, 0); break;
	case Camera::CAR:
		updateCarCamera(); break;
	}
}


// ------------------------------------------------------------
//
// Render stufff
//


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
			glActiveTexture(GL_TEXTURES[t]);
			glBindTexture(GL_TEXTURE_2D, TextureArray[part.mesh.mat.texIndices[t]]);
			glUniform1i(tex_loc[t], t);
		}
		loc = glGetUniformLocation(shader.getProgramIndex(), "mergeTextureWithColor");
		glUniform1i(loc, part.mesh.mat.mergeTextureWithColor);
		loc = glGetUniformLocation(shader.getProgramIndex(), "isHUD");
		glUniform1i(loc, part.mesh.mat.isHUD);

		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, part.mesh.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, part.mesh.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, part.mesh.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, part.mesh.mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, part.mesh.mat.texCount);
		pushMatrix(MODEL);

		translate(MODEL, obj->getX(), obj->getY(), obj->getZ());
		rotate(MODEL, obj->getAngle(), 0, 1, 0);
		rotate(MODEL, obj->getRollAngle(), 0, 0, -1);
		scale(MODEL, obj->getScaleX(), obj->getScaleY(), obj->getScaleZ());
		translate(MODEL, part.position[0], part.position[1], part.position[2]);
		rotate(MODEL, part.angle, part.rotationAxis[0], part.rotationAxis[1], part.rotationAxis[2]);
		scale(MODEL, part.scale[0], part.scale[1], part.scale[2]);

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
	}
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

	ortho(-1, 1, -1, 1, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (gameOver) {
		renderObject(gameOverQuad);
		renderObject(restartQuad);
	}
	else if (paused) {
		renderObject(pauseQuad);
	}

	renderObject(lives);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	popMatrix(MODEL);
	popMatrix(VIEW);
	popMatrix(PROJECTION);
}

void renderTextString(string text, float centerX, float centerY, float scale, float colorR, float colorG, float colorB) {
	float textScale = scale * windowHeight;
	float width = stringWidth(text) * textScale;
	float height = stringHeight(text) * textScale;
	RenderText(shaderText, text,
		windowWidth * centerX - width / 2,
		windowHeight * centerY - height / 2,
		textScale, colorR, colorG, colorB);
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
	
	float scale;
	float width;
	float height;
	if (gameOver) {
		/*scale = 0.003f * windowHeight;

		std::string text = "GAME OVER";
		width = stringWidth(text) * scale;
		height = stringHeight(text) * scale;
		RenderText(shaderText, text, (windowWidth - width) * 0.5f, windowHeight * 0.65f - height * 0.5f,
			scale, 1.0f, 0.3f, 0.2f);

		scale = 0.0015f * windowHeight;

		text = "Press R to restart.";
		width = stringWidth(text) * scale;
		height = stringHeight(text) * scale;
		RenderText(shaderText, text, (windowWidth - width) * 0.5f, windowHeight * 0.35f - height * 0.5f,
			scale, 1.0f, 1.0f, 1.0f);
		*/
		renderTextString("GAME OVER", 0.5f, 0.65f, 0.003f, 1.0f, 0.3f, 0.2f);
		renderTextString("Press R to restart.", 0.5f, 0.35f, 0.0015f, 1.0f, 1.0f, 1.0f);
	}
	else if (paused) {
		renderTextString("GAME PAUSED", 0.5f, 0.65f, 0.002f, 1.0f, 1.0f, 1.0f);
	}

	popMatrix(MODEL);
	popMatrix(VIEW);
	popMatrix(PROJECTION);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}



void renderScene(void) {

	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = (paused || gameOver) ? 0 : currentTime - lastTime;

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

	renderLights();

	for (Object* obj : gameObjects)
		obj->update(deltaTime);

	for (Object* obj : gameObjects)
		renderObject(obj);

	for (Object* obj : gameObjects)
		obj->handleCollision();

	if (showText) {
		renderHUDShapes();
		renderText();
	}

	if (lives->areEmpty())
		gameOver = true;
	
	glutSwapBuffers();

	lastTime = currentTime;
}

void restartGame() {
	paused = false;
	gameOver = false;
	lives->reset();
	car->reset();
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
	
	case 'r': // restart
		if (!pausedKey) {
			restartKey = true;
			restartGame();
		}
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
	case 'r':
		restartKey = false; break;
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
		camY = r *   						 sin(beta * DEG_TO_RAD);
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
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	tex_loc[0] = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc[1] = glGetUniformLocation(shader.getProgramIndex(), "texmap1");

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

	glGenTextures(NUM_TEXTURES, TextureArray);
	Texture2D_Loader(TextureArray, "img/stone.tga", STONE_TEX);
	Texture2D_Loader(TextureArray, "img/lightwood.tga", WOOD_TEX);
	Texture2D_Loader(TextureArray, "img/square-tiled-texture.jpg", CHECKERS_TEX);
	Texture2D_Loader(TextureArray, "img/orangeTex.png", ORANGE_TEX);
	Texture2D_Loader(TextureArray, "img/heart.png", LIFE_TEX);

	gameObjects.push_back(new Table());

	lives = new Lives(-0.6f, 0.8f, 0.08f, NUM_LIVES);

	car = new Car(&shader, 2.0f, 1.0f, lives);
	gameObjects.push_back(car);

	for (int o = 0; o < NUM_ORANGES; o++)
		gameObjects.push_back(new Orange(car));

	float butterPositions[] {
		14, 1,
		19, -0.75f,
		28, 1
	};

	float butterSizes[] {
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

	gameObjects.push_back(new Pawn());

	pauseQuad = new ScreenQuad(0, 0.3f, 0.15f, 0.2f);

	gameOverQuad = new ScreenQuad(0, 0.3f, 0.2f, 0.2f);
	restartQuad = new ScreenQuad(0, -0.3f, 0.1f, 0.15f);
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

	cameraProjection = Camera::CAR;

	createScene();
	
	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
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
	glutMouseWheelFunc (mouseWheel) ;
	

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



