
#pragma once
#include <GL/glew.h>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Texture_Loader.h"
#include "VertexAttrDef.h"
#include "AVTmathLib.h"



#define MAX_TEXTURES 8

enum texType {DIFFUSE, SPECULAR, NORMALS, BUMP};

struct Material {
	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	int texCount;
	int texIndices[2];
	bool mergeTextureWithColor;
	bool isHUD;
};
// A model can be made of many meshes. Each is stored  in the following structure
struct MyMesh {
	GLuint vao;
	GLuint texUnits[MAX_TEXTURES];
	texType texTypes[4];
	float transform[16];
	GLuint numIndexes;
	unsigned int type;
	struct Material mat;
};

//std::vector<struct MyMesh> createMeshFromAssimp(const std::string& filepath);
MyMesh createCube();
MyMesh createQuad(float size_x, float size_y);
MyMesh createSphere(float radius, int divisions);
MyMesh createTorus(float innerRadius, float outerRadius, int rings, int sides);
MyMesh createCylinder(float height, float radius, int sides);
MyMesh createCone(float height, float baseRadius, int sides);
MyMesh createPawn();
MyMesh computeVAO(int numP, float *p, float *pfloatoints, int sides, float smoothCos);
int revSmoothNormal2(float *p, float *nx, float *ny, float smoothCos, int beginEnd);
float *circularProfile(float minAngle, float maxAngle, float radius, int divisions, float transX= 0.0f, float transY = 0.0f);
void ComputeTangentArray(int vertexCount, float *vertex, float *normal, float *texcoord, GLuint indexesCount, GLuint *faceIndex, float *tangent);