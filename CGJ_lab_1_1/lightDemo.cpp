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

#include "Object.h"
#include "Car.h"
#include "constants.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "Lab 1.1"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

int lastTime = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

//File with the font
const string font_name = "fonts/arial.ttf";

//Vector with meshes
vector<struct MyMesh> myMeshes;

vector<float> xPositions;
vector<float> yPositions;
vector<float> zPositions;

vector<float> xScales;
vector<float> yScales;
vector<float> zScales;

vector<float> angles;
vector<float> xRotations;
vector<float> yRotations;
vector<float> zRotations;

Car* car;

const enum class CAMERA {
	ORTHOGONAL, PERSPECTIVE, CAR
};

CAMERA cameraProjection;

float camRatio;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint tex_loc, tex_loc1, tex_loc2;
	
// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 10.0f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};


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
	case CAMERA::ORTHOGONAL:
		ortho(-50 * camRatio, 50 * camRatio, -50, 50, 1, 1500);
		break;
	case CAMERA::PERSPECTIVE:
	case CAMERA::CAR:
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
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix

	camRatio = (1.0f * w) / h;

	setCameraProjection();
}


// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	switch (cameraProjection) {
	case CAMERA::ORTHOGONAL:
		lookAt(0, 100, 0, 0, 0, 0, 1, 0, 0); break;
	case CAMERA::PERSPECTIVE:
		lookAt(-66, 30, -66, 0, 0, 0, 0, 1, 0); break;
	case CAMERA::CAR:
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

		break;
	}
	
	// use our shader
	glUseProgram(shader.getProgramIndex());

	//send the light position in eye coordinates
	//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	float res[4];
	multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos_uniformId, 1, res);

	int objId = 0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	for (int i = 0; i < myMeshes.size(); ++i) {
		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[objId].mat.shininess);
		pushMatrix(MODEL);
		translate(MODEL, xPositions[i], yPositions[i], zPositions[i]);
		rotate(MODEL, angles[i], xRotations[i], yRotations[i], zRotations[i]);
		scale(MODEL, xScales[i], yScales[i], zScales[i]);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(myMeshes[objId].vao);
			
		if (!shader.isProgramValid()) {
			std::printf("Program Not Valid!\n");
			exit(1);	
		}
		glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		objId ++;
	}

	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - lastTime;

	vector<Object::Part>* parts = car->update(deltaTime);

	for (const Object::Part& part : *parts) {
		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, part.mesh.mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, part.mesh.mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, part.mesh.mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, part.mesh.mat.shininess);
		pushMatrix(MODEL);
		
		translate(MODEL, car->getX(), car->getY(), car->getZ());
		rotate(MODEL, car->getAngle(), 0, 1, 0);
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

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	RenderText(shaderText, "X " + std::to_string(car->getX()), 25.0f, 125.0f, 0.5f, 0.5f, 0.5f, 0.8f);
	RenderText(shaderText, "Z " + std::to_string(car->getZ()), 25.0f, 100.0f, 0.5f, 0.5f, 0.5f, 0.8f);
	RenderText(shaderText, "Angle " + std::to_string(car->getAngle()), 25.0f, 75.0f, 0.5f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "Speed " + std::to_string(car->getSpeed()), 25.0f, 50.0f, 0.5f, 0.5f, 0.2f, 0.8f);
	RenderText(shaderText, "Angular speed " + std::to_string(car->getAngularSpeed()), 25.0f, 25.0f, 0.5f, 0.5f, 0.8f, 0.2f);
	
	//RenderText(shaderText, "CGJ Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

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
		cameraProjection = CAMERA::ORTHOGONAL; setCameraProjection(); break;
	case '2':
		cameraProjection = CAMERA::PERSPECTIVE; setCameraProjection(); break;
	case '3':
		cameraProjection = CAMERA::CAR; setCameraProjection(); break;

	case 'w':
		car->accelerate(true); break;
	case 's':
		car->accelerateBack(true); break;
	case 'a':
		car->turnLeft(true); break;
	case 'd':
		car->turnRight(true); break;
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
	}
}



// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	if (cameraProjection == CAMERA::CAR) {
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
	if (cameraProjection == CAMERA::CAR) {
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
	if (cameraProjection == CAMERA::CAR) {
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
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	
	std::printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	std::printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());
	
	return(shader.isProgramLinked() && shaderText.isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void createTable() {
	MyMesh amesh;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	xScales.push_back(100.0f);
	yScales.push_back(0.2f);
	zScales.push_back(100.0f);

	xPositions.push_back(-50.0f);
	yPositions.push_back(-0.2f);
	zPositions.push_back(-50.0f);

	angles.push_back(0);
	xRotations.push_back(1.0f);
	yRotations.push_back(0);
	zRotations.push_back(0);
}

void createLimits() {
	MyMesh amesh;

	float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	float diff[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 300.0f;
	int texcount = 0;

	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	
	float signs[]{ -1, 1 };

	for (int sign = 0; sign < 2; sign++) {
		float zSign = signs[sign];
		// Left limit
		myMeshes.push_back(amesh);

		xScales.push_back(80.0f);
		yScales.push_back(0.01f);
		zScales.push_back(0.5f);

		xPositions.push_back(-40.0f);
		yPositions.push_back(0);
		zPositions.push_back(2.0f * zSign - 0.25f);

		angles.push_back(0);
		xRotations.push_back(1.0f);
		yRotations.push_back(0);
		zRotations.push_back(0);
	}
}

void createCheerios() {
	MyMesh amesh;

	float amb[] = { 0.6f, 0.48f, 0.0f, 1.0f };
	float diff[] = { 0.8f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int texcount = 0;

	amesh = createTorus(0.2f, 0.4f, 12, 12);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	
	float signs[]{ -1, 1 };

	for (int sign = 0; sign < 2; sign++) {
		float zSign = signs[sign];

		for (float x = -40.0f; x <= 40.0f; x += 2) {
			myMeshes.push_back(amesh);

			xScales.push_back(1.0f);
			yScales.push_back(1.0f);
			zScales.push_back(1.0f);

			xPositions.push_back(x);
			yPositions.push_back(0.1f);
			zPositions.push_back(3.0f * zSign);

			angles.push_back(0);
			xRotations.push_back(1.0f);
			yRotations.push_back(0);
			zRotations.push_back(0);
		}
	}
}

void createRoad() {
	createLimits();
	createCheerios();
}

void createButter() {
	MyMesh amesh;

	float amb[] = { 0.6f, 0.48f, 0.0f, 1.0f };
	float diff[] = { 0.8f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int texcount = 0;

	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;

	float positions[]{
		14, 1,
		19, -0.75f,
		28, 1
	};

	float rotations[]{
		0,
		45,
		90
	};

	int numButters = sizeof(rotations) / sizeof(float);

	for (int i = 0; i < numButters; i++) {
		myMeshes.push_back(amesh);

		xScales.push_back(2.0f);
		yScales.push_back(0.5f);
		zScales.push_back(1.0f);

		xPositions.push_back(-1.0f + positions[2 * i]);
		yPositions.push_back(0);
		zPositions.push_back(-0.5f + positions[2 * i + 1]);

		angles.push_back(rotations[i]);
		xRotations.push_back(0);
		yRotations.push_back(1.0f);
		zRotations.push_back(0);
	}
}

void createOrange() {
	MyMesh amesh;

	float amb[] = { 0.8f, 0.4f, 0.0f, 1.0f };
	float diff[] = { 0.6f, 0.6f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int texcount = 0;

	amesh = createSphere(2, 12);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;

	float positions[]{
		14, 5,
		19, -4,
		28, 5
	};

	int numOranges = (sizeof(positions) / 2) / sizeof(float);

	for (int i = 0; i < numOranges; i++) {
		myMeshes.push_back(amesh);

		xScales.push_back(1.0f);
		yScales.push_back(1.0f);
		zScales.push_back(1.0f);

		xPositions.push_back(positions[2 * i]);
		yPositions.push_back(2.0f);
		zPositions.push_back(positions[2 * i + 1]);

		angles.push_back(0);
		xRotations.push_back(1.0f);
		yRotations.push_back(0);
		zRotations.push_back(0);
	}
}

void createScene() {
	/*

	// create geometry and VAO of the sphere
	amesh = createSphere(1.0f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	float amb1[]= {0.3f, 0.0f, 0.0f, 1.0f};
	float diff1[] = {0.8f, 0.1f, 0.1f, 1.0f};
	float spec1[] = {0.9f, 0.9f, 0.9f, 1.0f};
	shininess=500.0;

	// create geometry and VAO of the cylinder
	amesh = createCylinder(1.5f, 0.5f, 20);
	memcpy(amesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the cube
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);
	*/

	createTable();
	createRoad();
	createButter();
	createOrange();
	
	car = new Car();
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
	freeType_init(font_name);

	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
	camZ = r * cos(alpha * DEG_TO_RAD) * cos(beta * DEG_TO_RAD);
	camY = r *   						     sin(beta * DEG_TO_RAD);

	cameraProjection = CAMERA::ORTHOGONAL;

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



