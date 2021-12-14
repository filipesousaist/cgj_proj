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

using namespace std;

#define CAPTION "Lab 1.1"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text
VSShaderLib shaderT;  //Textures

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
GLint tex_loc, tex_loc1, tex_loc2, tex_loc3;
GLint texMode_uniformId;

GLuint TextureArray[4];
	
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

// Time variables
float deltaTime;
float previousTime;

// Car properties
float speed = 0;
float accel = 0;
float uniformAccel = 5.0f;
float maxSpeed = 100.0f;

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
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

void setCameraProjection() {
	loadIdentity(PROJECTION);

	switch (cameraProjection)
	{
	case CAMERA::ORTHOGONAL:
		ortho(-50 * camRatio, 50 * camRatio, -50, 50, -50, 50);
		break;
	case CAMERA::PERSPECTIVE:
		perspective(53.13f, camRatio, 0.1f, 1000.0f);
		break;
	case CAMERA::CAR:
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

	//Calculate delta time between frames
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	lookAt(camX, camY, camZ, 0,0,0, 0,1,0);
	// use our shader
	glUseProgram(shaderT.getProgramIndex());

	//Associar os Texture Units aos Objects Texture
	//stone.tga loaded in TU0; checker.tga loaded in TU1;  lightwood.tga loaded in TU2

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);

	

	//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);


		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId, 1, res);

	int objId=0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	for (int i = 0; i < myMeshes.size(); ++i) {
		// send the material
		loc = glGetUniformLocation(shaderT.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
		loc = glGetUniformLocation(shaderT.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
		loc = glGetUniformLocation(shaderT.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shaderT.getProgramIndex(), "mat.shininess");
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
		glUniform1i(texMode_uniformId, myMeshes[objId].mat.texCount);

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
	//ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	//RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	//RenderText(shaderText, "CGJ Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	glutSwapBuffers();
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
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
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

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

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
		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

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

	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");

	std::printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	std::printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	// Shader for table
	shaderT.init();
	shaderT.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/texture_demo.vert");
	shaderT.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/texture_demo.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderT.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shaderT.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shaderT.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shaderT.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shaderT.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shaderT.getProgramIndex(), "texMode"); // different modes of texturing
	pvm_uniformId = glGetUniformLocation(shaderT.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shaderT.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shaderT.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shaderT.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shaderT.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shaderT.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shaderT.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");

	printf("InfoLog for Shader\n%s\n\n", shaderT.getAllInfoLogs().c_str());

	return(shader.isProgramLinked() && shaderText.isProgramLinked() && shaderT.isProgramLinked());
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
	int texcount = 2;

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

void createCarBody() {
	MyMesh amesh;

	float amb[] = { 0.0f, 0.6f, 0.0f, 1.0f };
	float diff[] = { 0.2f, 0.8f, 0.2f, 1.0f };
	float spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
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

	// Bottom part
	myMeshes.push_back(amesh);

	xScales.push_back(2.0f);
	yScales.push_back(0.5f);
	zScales.push_back(1.0f);

	xPositions.push_back(-1.0f);
	yPositions.push_back(0.25f);
	zPositions.push_back(-0.5f);

	angles.push_back(0);
	xRotations.push_back(1.0f);
	yRotations.push_back(0);
	zRotations.push_back(0);

	// Cockpit

	myMeshes.push_back(amesh);

	xScales.push_back(0.8f);
	yScales.push_back(0.4f);
	zScales.push_back(0.6f);

	xPositions.push_back(-0.5f);
	yPositions.push_back(0.65f);
	zPositions.push_back(-0.3f);

	angles.push_back(0);
	xRotations.push_back(1.0f);
	yRotations.push_back(0);
	zRotations.push_back(0);
}

void createCarWheels() {
	MyMesh amesh;

	float amb[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	float diff[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 80.0f;
	int texcount = 0;

	amesh = createTorus(0.05f, 0.25f, 20, 12);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;

	float signs[]{ -1, 1 };

	for (int x = 0; x < 2; x ++) {
		float xSign = signs[x];
		for (int z = 0; z < 2; z ++) {
			float zSign = signs[z];

			myMeshes.push_back(amesh);

			xScales.push_back(1.0f);
			yScales.push_back(1.0f);
			zScales.push_back(1.0f);

			xPositions.push_back(0.5f * xSign);
			yPositions.push_back(0.25f);
			zPositions.push_back(0.5f * zSign);

			angles.push_back(90);
			xRotations.push_back(1.0f);
			yRotations.push_back(0);
			zRotations.push_back(0);
		}	
	}
}

void createCar() {
	createCarBody();
	createCarWheels();
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
	int texcount = 3;

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

	//Texture Object definition

	glGenTextures(4, TextureArray);
	Texture2D_Loader(TextureArray, "orangeTex.png", 0);
	Texture2D_Loader(TextureArray, "checker.png", 1);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2);
	Texture2D_Loader(TextureArray, "stone.tga", 3);

	createTable();
	createCar();
	createRoad();
	createButter();
	createOrange();
}


void updateCar() {
	speed = fminf(speed + (uniformAccel * deltaTime), 0);
	speed = fmaxf(speed, maxSpeed);

	//update directions

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
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

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



