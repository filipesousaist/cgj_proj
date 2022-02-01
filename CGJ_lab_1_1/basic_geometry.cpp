/* --------------------------------------------------
Basic Revolution Geometry 
 *
 * based on vsResourceLib.cpp from Lighthouse3D
 *It includes ComputeTangentArray routine
 *
 João Madeiras Pereira
----------------------------------------------------*/
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <unordered_map>


// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>
#include "geometry.h"
#include "cube.h"


GLuint VboId[2];


using namespace std;

// Create an instance of the Importer class
Assimp::Importer importer;
// the global Assimp scene object
const aiScene* scene = NULL;
// scale factor for the Assimp model to fit in the window
float scaleFactor;

/* Directory name containing the OBJ file. The OBJ filename should be the same*/
char model_dir[50];


// unordered map which maps image filenames to texture units TU. This map is filled in the  LoadGLTexturesTUs()
unordered_map<std::string, GLuint> textureIdMap;


#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void get_bounding_box_for_node(const aiNode* nd, aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n], min, max);
	}
}


void get_bounding_box(aiVector3D* min, aiVector3D* max)
{

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode, min, max);
}

bool Import3DFromFile(const std::string& pFile)
{

	scene = importer.ReadFile(pFile, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if (!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	printf("Import of scene %s succeeded.\n", pFile.c_str());

	aiVector3D scene_min, scene_max, scene_center;
	get_bounding_box(&scene_min, &scene_max);
	float tmp;
	tmp = scene_max.x - scene_min.x;
	tmp = scene_max.y - scene_min.y > tmp ? scene_max.y - scene_min.y : tmp;
	tmp = scene_max.z - scene_min.z > tmp ? scene_max.z - scene_min.z : tmp;
	scaleFactor = 1.f / tmp;

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}


bool LoadGLTexturesTUs(const aiScene* scene)  // Create OGL textures objects and maps them to texture units.  
{
	aiString path;	// filename
	string filename;

	/* scan scene's materials for textures */
	for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
	{
		// o fragment shader suporta material com duas texturas difusas, 1 especular e 1 normal map
		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_DIFFUSE); i++) {

			scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, i, &path);
			filename = model_dir;
			filename.append(path.data);
			//fill map with textures, OpenGL image ids set to 0
			textureIdMap[filename] = 0;
		}

		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_SPECULAR); i++) {
			scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, i, &path);
			filename = model_dir;
			filename.append(path.data);
			textureIdMap[filename] = 0;
		}

		for (unsigned int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_NORMALS); i++) {
			scene->mMaterials[m]->GetTexture(aiTextureType_NORMALS, i, &path);
			filename = model_dir;
			filename.append(path.data);
			textureIdMap[filename] = 0;
		}

	}

	int numTextures = textureIdMap.size();
	printf("numeros de mapas %d\n", numTextures);
	if (numTextures) {
		GLuint* textureIds = new GLuint[numTextures];
		glGenTextures(numTextures, textureIds); /* Texture name generation */

		/* get iterator */
		unordered_map<std::string, GLuint>::iterator itr = textureIdMap.begin();
		filename = (*itr).first;  // get filename

		//create the texture objects array and asssociate them with TU and place the TU in the key value of the map
		for (int i = 0; itr != textureIdMap.end(); ++i, ++itr)
		{
			filename = (*itr).first;  // get filename
			glActiveTexture(GL_TEXTURE0 + i);
			Texture2D_Loader(textureIds, filename.c_str(), i);  //it already performs glBindTexture(GL_TEXTURE_2D, textureIds[i])
			(*itr).second = i;	  // save texture unit for filename in map
			//printf("textura = %s  TU = %d\n", filename.c_str(), i);
		}

		//Cleanup
		delete[] textureIds;
	}
	return true;
}

/// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
//void Color4f(const aiColor4D *color)
//{
//	glColor4f(color->r, color->g, color->b, color->a);
//}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D* c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

vector<struct MyMesh> createMeshFromAssimp(const std::string& pFile) {

	if (!Import3DFromFile(pFile))
		exit(1);

	strcpy(model_dir, pFile.c_str());

	vector<struct MyMesh> myMeshes;
	struct MyMesh aMesh;
	GLuint buffer;

	printf("Cena: numero total de malhas = %d\n", scene->mNumMeshes);

	LoadGLTexturesTUs(scene); //it creates the unordered map which maps image filenames to texture units TU

	// For each mesh
	for (unsigned int n = 0; n < scene->mNumMeshes; ++n)
	{
		const aiMesh* mesh = scene->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		unsigned int* faceArray;
		faceArray = (unsigned int*)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			faceIndex += 3;
		}
		struct MyMesh aMesh;
		aMesh.numIndexes = mesh->mNumFaces * 3;
		aMesh.type = GL_TRIANGLES;
		aMesh.mat.texCount = 0;

		// generate Vertex Array for mesh
		glGenVertexArrays(1, &(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * aMesh.numIndexes, faceArray, GL_STATIC_DRAW);

		// buffer for vertex positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
			glVertexAttribPointer(VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(NORMAL_ATTRIB);
			glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffers for vertex tangents and bitangents
		if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mTangents, GL_STATIC_DRAW);
			glEnableVertexAttribArray(TANGENT_ATTRIB);
			glVertexAttribPointer(TANGENT_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mBitangents, GL_STATIC_DRAW);
			glEnableVertexAttribArray(BITANGENT_ATTRIB);
			glVertexAttribPointer(BITANGENT_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			float* texCoords = (float*)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

				texCoords[k * 2] = mesh->mTextureCoords[0][k].x;
				texCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;

			}
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
			glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// create material; each mesh has ONE material
		aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];

		aiString texPath;	//contains filename of texture

		string filename;
		GLuint TU;
		unsigned int TUcount = 0;

		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_DIFFUSE); i++) {

			mtl->GetTexture(aiTextureType_DIFFUSE, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = DIFFUSE;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}

		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_SPECULAR); i++) {
			mtl->GetTexture(aiTextureType_SPECULAR, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = SPECULAR;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}
		for (unsigned int i = 0; i < mtl->GetTextureCount(aiTextureType_NORMALS); i++) {
			mtl->GetTexture(aiTextureType_NORMALS, i, &texPath);
			filename = model_dir;
			filename.append(texPath.data);
			TU = textureIdMap[filename];

			aMesh.texUnits[TUcount] = TU;
			aMesh.texTypes[TUcount] = NORMALS;
			aMesh.mat.texCount = TUcount + 1;
			TUcount++;
		}


		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			color4_to_float4(&diffuse, c);
		memcpy(aMesh.mat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
			color4_to_float4(&ambient, c);
		memcpy(aMesh.mat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
			color4_to_float4(&specular, c);
		memcpy(aMesh.mat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
			color4_to_float4(&emission, c);
		memcpy(aMesh.mat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMesh.mat.shininess = shininess;

		myMeshes.push_back(aMesh);
	}
	// cleaning up
	textureIdMap.clear();

	return(myMeshes);
}

MyMesh createQuad(float size_x, float size_y) {
	
	int i;
	float vert[16];
	MyMesh amesh;
	amesh.numIndexes = 2*3;

	memcpy(vert, quad_vertices, sizeof(float) * 16);

	for(i=0; i< 4; i++) {
		vert[i*4] *= size_x;
		vert[i*4+1] *= size_y;
	}

	glGenVertexArrays(1, &(amesh.vao));
	glBindVertexArray(amesh.vao);

	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices)+sizeof(quad_normals)+sizeof(quad_texCoords),NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_vertices), vert);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vertices), sizeof(quad_normals), quad_normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vertices)+ sizeof(quad_normals), sizeof(quad_texCoords), quad_texCoords);
    
    glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glVertexAttribPointer(NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)sizeof(quad_vertices));
    glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
    glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(quad_vertices)+ sizeof(quad_normals)));
    
    //index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * amesh.numIndexes, quad_faceIndex , GL_STATIC_DRAW);
    
    // unbind the VAO
    glBindVertexArray(0);
  
	amesh.type = GL_TRIANGLES;
	return(amesh);
}

MyMesh createCube() {
	MyMesh amesh;
	amesh.numIndexes = faceCount *3;

	glGenVertexArrays(1, &(amesh.vao));
	glBindVertexArray(amesh.vao);

	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	float allData[sizeof(vertices) + sizeof(normals) + sizeof(texCoords) + sizeof(tangents)];
	memcpy(allData, vertices, sizeof(vertices));
	memcpy(allData + sizeof(vertices) / sizeof(float), normals, sizeof(normals));
	memcpy(allData + (sizeof(vertices) + sizeof(normals)) / sizeof(float), texCoords, sizeof(texCoords));
	memcpy(allData + (sizeof(vertices) + sizeof(normals) + sizeof(texCoords)) / sizeof(float), tangents, sizeof(tangents));

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals)+sizeof(texCoords)+sizeof(tangents),allData,GL_STATIC_DRAW);

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(NORMAL_ATTRIB);
	glVertexAttribPointer(NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)sizeof(vertices));
	glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(vertices)+ sizeof(normals)));
	glEnableVertexAttribArray(TANGENT_ATTRIB);
	glVertexAttribPointer(TANGENT_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(vertices) + sizeof(normals) + sizeof(texCoords)));

	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * amesh.numIndexes, faceIndex , GL_STATIC_DRAW);

// unbind the VAO
	glBindVertexArray(0);
	
	amesh.type = GL_TRIANGLES;
	return(amesh);
}


MyMesh createSphere(float radius, int divisions) {

	float *p = circularProfile(-3.14159f/2.0f, 3.14159f/2.0f, radius, divisions);
	return(computeVAO(divisions+1, p+2, p, divisions*2, 0.0f));
}


MyMesh createTorus(float innerRadius, float outerRadius, int rings, int sides) {

	float tubeRadius = (outerRadius - innerRadius) * 0.5f;
	float *p = circularProfile(-3.14159f, 3.14159f, tubeRadius, sides, innerRadius + tubeRadius);
	return(computeVAO(sides+1, p+2, p, rings, 0.0f));
}


MyMesh createCylinder(float height, float radius, int sides) {

	float p[] = {
			-radius,	-height*0.5f, 
			0.0f,		-height*0.5f, 
			radius,		-height*0.5f,  
			radius,		 height*0.5f,  
			0.0f,		 height*0.5f,
			-radius,	 height*0.5f
	};

	return(computeVAO(4, p+2, p, sides, 0.0f));
}

MyMesh createCone(float height, float baseRadius, int sides) {

	float v[2];
	v[0] = -baseRadius;
	v[1] = height;

	std::vector<float> p;

	p.push_back(-baseRadius);	p.push_back(0.0f);
	p.push_back(0.0f);			p.push_back(0.0f);
	p.push_back(baseRadius);	p.push_back(0.0f);
	int max = (int)(1 + height/ (baseRadius*2*3.14159 / sides)) ;
	for (int i = 0; i < max; ++i) {
	
		p.push_back(baseRadius - i * baseRadius  / max); p.push_back( i * height  / max);
	}
	p.push_back(0.0f);			p.push_back(height);
	p.push_back(-baseRadius);	p.push_back(height * 2.0f);
	//float p[(sides+3)*2] = {
	//		-baseRadius,	0.0, 
	//		0.0f,			0.0, 
	//		baseRadius,		0.0f,
	//		baseRadius + v[0] * 0.1,		v[1] * 0.1,
	//		baseRadius + v[0] * 0.2,		v[1] * 0.2,
	//		baseRadius + v[0] * 0.3,		v[1] * 0.3,
	//		baseRadius + v[0] * 0.4,		v[1] * 0.4,
	//		baseRadius + v[0] * 0.5,		v[1] * 0.5,
	//		baseRadius + v[0] * 0.6,		v[1] * 0.6,
	//		baseRadius + v[0] * 0.7,		v[1] * 0.7,
	//		baseRadius + v[0] * 0.8,		v[1] * 0.8,
	//		baseRadius + v[0] * 0.9,		v[1] * 0.9,
	//		0.0f,			height,  
	//		-baseRadius,	height*2.0f,
	//	};

	return(computeVAO((p.size()-4)/2, &(p[2]), &(p[0]), sides, 0.0f));
}


MyMesh createPawn() {

		float p[] = {0.0f, 0.0f, 
					  0.98f, 0.0f, 
					  0.98f, 0.01f,
					  0.99f, 0.02f,
					  1.0f , 0.02f,
					  1.0f , 0.3f,
					  0.99f, 0.31f,
					  0.98f, 0.32f,
					  0.93f, 0.32f,

					  0.95f, 0.38f,
					  0.965f, 0.44f,
					  0.97f, 0.48f,
					  0.965f, 0.52f,
					  0.95f, 0.56f,
					  0.89f, 0.62f,
					  0.83f, 0.66f,
					  0.75f, 0.70f,
					  0.66f, 0.735f,
					  0.65f, 0.74f,

					  
					  0.611f, 0.83f,
					  0.5f,   0.83f,

					  0.35f,  2.0f,

					  0.58f,  2.075f,
					  0.610f, 2.10f,
					  0.6225f, 2.1125f,
					  0.625f, 2.125f,
					  0.6225f, 2.1375f,
					  0.610f, 2.15f,
					  0.58f,  2.175f,

					  0.32f, 2.27f,
					  0.46f, 2.38f,
					  0.56f, 2.514f,
					  0.628f, 2.67f,
					  0.65f, 2.84f,

					  0.628f, 3.01f,
					  0.56f, 3.164f,
					  0.46f, 3.3f,
					  0.32f, 3.40f,
					  0.168f, 3.467f,
					  0.0f, 3.486f
					 };
		int numP=40;
		int sides=30;
		int closed = 1;
		float smoothCos = 0.6f;

	int i; 
	// two extra points are added for normal computation

	float *points = (float *)malloc(sizeof(float) * (numP+2) * 2);

	for(i=2;i<(numP+1)*2;i++) {
		points[i] = p[i-2];
	}

	// distinguishes between closed curves and open curves
	// for normal computation
	int numPoints = numP + 2;

	if (closed) {

		points[0] = p[(numP-2)*2];
		points[1] = p[(numP-2)*2+1];

		points[(numPoints-1)*2]     = p[2];
		points[(numPoints-1)*2 + 1] = p[3];
	}
	else {

		points[0] = points[2] + (points[2]-points[4]);
		points[1] = points[3] + (points[3]-points[5]);

		points[(numPoints-1)*2]     = points[(numPoints-2)*2]     + 
											(points[(numPoints-2)*2]     - points[(numPoints-3)*2]);
		points[(numPoints-1)*2 + 1] = points[(numPoints-2)*2 + 1] + 
											(points[(numPoints-2)*2 + 1] - points[(numPoints-3)*2 + 1]);
	}

	return(computeVAO(numP, p, points, sides, smoothCos));
}

MyMesh computeVAO(int numP, float *p, float *points, int sides, float smoothCos) {
	// Compute and store vertices
	
	int numSides = sides;
	int numPoints = numP + 2;

	int numVertices = numP * 2 * (numSides + 1);

	float *vertex = (float *)malloc(sizeof(float) * 4 * numVertices);
	float *normal = (float *)malloc(sizeof(float) * 4 * numVertices);
	float *textco = (float *)malloc(sizeof(float) * 4 * numVertices);
	float *tangent = (float *)malloc(sizeof(float) * 4 * numVertices);

	float inc = 2 * 3.14159f / (numSides);
	float nx,ny;
	float delta;
	int smooth;
	std::vector<int> smoothness;
	int k = 0;
	for(int i=0; i < numP; i++) {
		revSmoothNormal2(points+(i*2),&nx,&ny, smoothCos, 0);
		for(int j=0; j<=numSides;j++) {

			if ((i == 0 && p[0] == 0.0f) || ( i == numP-1 && p[(i+1)*2] == 0.0))
				delta = inc * 0.5f;
			else
				delta = 0.0f;

			normal[((k)*(numSides+1) + j)*4]   = nx * cos(j*inc+delta);
			normal[((k)*(numSides+1) + j)*4+1] = ny;
			normal[((k)*(numSides+1) + j)*4+2] = nx * sin(-j*inc+delta);
			normal[((k)*(numSides+1) + j)*4+3] = 0.0f;

			vertex[((k)*(numSides+1) + j)*4]   = p[i*2] * cos(j*inc);
			vertex[((k)*(numSides+1) + j)*4+1] = p[(i*2)+1];
			vertex[((k)*(numSides+1) + j)*4+2] = p[i*2] * sin(-j*inc);
			vertex[((k)*(numSides+1) + j)*4+3] = 1.0f;

			textco[((k)*(numSides+1) + j)*4]   = ((j+0.0f)/numSides);
			textco[((k)*(numSides+1) + j)*4+1] = (i+0.0f)/(numP-1);
			textco[((k)*(numSides+1) + j)*4+2] = 0;
			textco[((k)*(numSides+1) + j)*4+3] = 1.0f;
		}
		k++;
		if (i < numP-1) {
			smooth = revSmoothNormal2(points+((i+1)*2),&nx,&ny, smoothCos, 1);

			if (!smooth) {
				smoothness.push_back(1);
				for(int j=0; j<=numSides;j++) {

				normal[((k)*(numSides+1) + j)*4]   = nx * cos(j*inc);
				normal[((k)*(numSides+1) + j)*4+1] = ny;
				normal[((k)*(numSides+1) + j)*4+2] = nx * sin(-j*inc);
				normal[((k)*(numSides+1) + j)*4+3] = 0.0f;

				vertex[((k)*(numSides+1) + j)*4]   = p[(i+1)*2] * cos(j*inc);
				vertex[((k)*(numSides+1) + j)*4+1] = p[((i+1)*2)+1];
				vertex[((k)*(numSides+1) + j)*4+2] = p[(i+1)*2] * sin(-j*inc);
				vertex[((k)*(numSides+1) + j)*4+3] = 1.0f;

				textco[((k)*(numSides+1) + j)*4]   = ((j+0.0f)/numSides);
				textco[((k)*(numSides+1) + j)*4+1] = (i+1+0.0f)/(numP-1);
				textco[((k)*(numSides+1) + j)*4+2] = 0;
				textco[((k)*(numSides+1) + j)*4+3] = 1.0f;
				}
				k++;
			}
			else
				smoothness.push_back(0);
		}
	}

	unsigned int *faceIndex = (unsigned int *)malloc(sizeof(unsigned int) * (numP-1) * (numSides+1 ) * 6);
	unsigned int count = 0;
	k = 0;
	for (int i = 0; i < numP-1; ++i) {
		for (int j = 0; j < numSides; ++j) {
		
			/*if (i != 0 || p[0] != 0.0)*/ {
				faceIndex[count++] = k * (numSides+1) + j;
				faceIndex[count++] = (k+1) * (numSides+1) + j + 1;
				faceIndex[count++] = (k+1) * (numSides+1) + j;
			}
			/*if (i != numP-2 || p[(numP-1)*2] != 0.0)*/ {
				faceIndex[count++] = k * (numSides+1) + j;
				faceIndex[count++] = k * (numSides+1) + j + 1;
				faceIndex[count++] = (k+1) * (numSides+1) + j + 1;
			}

		}
		k++;
		k += smoothness[i];	
	}

	MyMesh amesh;
	amesh.numIndexes = count;


	/* Calculate the tangent array*/
	ComputeTangentArray(numVertices, vertex, normal, textco, amesh.numIndexes, faceIndex, tangent);

	glGenVertexArrays(1, &(amesh.vao));
	glBindVertexArray(amesh.vao);

	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*4*4, NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(float)*4, vertex);
		glBufferSubData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*4, numVertices*sizeof(float)*4, normal);
		glBufferSubData(GL_ARRAY_BUFFER, numVertices*sizeof(float)*4*2, numVertices*sizeof(float)*4, textco);
		glBufferSubData(GL_ARRAY_BUFFER, numVertices * sizeof(float) * 4 * 3, numVertices * sizeof(float) * 4, tangent);

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(NORMAL_ATTRIB);
	glVertexAttribPointer(NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(numVertices*sizeof(float)*4));
	glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(numVertices*sizeof(float)*4*2));
	glEnableVertexAttribArray(TANGENT_ATTRIB);
	glVertexAttribPointer(TANGENT_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(numVertices * sizeof(float) * 4 * 3));

	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * amesh.numIndexes, faceIndex , GL_STATIC_DRAW);

// unbind the VAO
	glBindVertexArray(0);

	amesh.type = GL_TRIANGLES;
	return(amesh);
}

int revSmoothNormal2(float *p, float *nx, float *ny, float smoothCos, int beginEnd) {

	float v1x,v1y,v2x,v2y,x,y,norm;
	float auxX, auxY, angle;

	auxX = p[0] - p[2];
	auxY = p[1] - p[3];
	v1x = -auxY;
	v1y = auxX;
	norm=sqrt((v1x*v1x) + (v1y*v1y));
	v1x /= norm;
	v1y /= norm;

	auxX = p[2] - p[4];
	auxY = p[3] - p[5];
	v2x = -auxY;
	v2y = auxX;
	norm=sqrt((v2x*v2x) + (v2y*v2y));
	v2x /= norm;
	v2y /= norm;

	angle = v1x * v2x + v1y * v2y;

 	if (angle > smoothCos) {
		x = v1x + v2x;
		y = v1y + v2y;
	}
	else if (beginEnd == 0) {
		x = v2x;
		y = v2y;
	}
	else  {
		x = v1x;
		y = v1y;
	
	}

	norm=sqrt(x*x+ y*y);

	assert(norm!=0);
	x /= norm;
	y /= norm;

	*nx = x;
	*ny = y;
	if (angle > smoothCos)
		return 1;
	else
		return 0;
}

float *circularProfile(float minAngle, float maxAngle, float radius, int divisions, float transX, float transY ) {

	float *p = (float *)malloc(sizeof(float) * 2 * (divisions+3));
	float step = (maxAngle - minAngle)/ divisions;

	for (int i = 0, k = -1; i < divisions+3; ++i,++k) {
	
		p[i*2]   = radius * cos(minAngle + k * step) + transX;
		p[i*2+1] = radius * sin(minAngle + k * step) + transY;
//		printf("%f %f\n", p[i*2], p[i * 2 + 1]);
	}
	return p;
}

void	ComputeTangentArray(int vertexCount, float *vertex, float *normal, float *texcoord, GLuint indexesCount, GLuint *faceIndex, float *tangent) {

	GLuint iV0, iV1, iV2;
	GLuint tV0, tV1, tV2;
	float aux_vec[] = { 0.0f, 0.0f, 0.0f };

	/* Auxiliary arrays tan1 and tan 2 for computing the tangent array*/
	float *tan1 = (float *)malloc(sizeof(float) * 3 * vertexCount * 2);
	float *tan2 = tan1 + 3 * vertexCount;
	memset((void *)tan1, 0, sizeof(float) * 3 * vertexCount * 2);

	//Ciclo ao nível de triângulos: i representa o numero de indices
	for (GLuint i = 0; i < indexesCount; i += 3) {

		//iV0 - indice do vértice no vertex array: cada vértice são 4 floats
		iV0 = faceIndex[i] * 4;
		iV1 = faceIndex[i + 1] * 4;
		iV2 = faceIndex[i + 2] * 4;

		//printf("indices triangulo:[%d %d %d]\n", iV0 / 4, iV1 / 4, iV2 / 4);
		//tV0 - indice do vértice no auxiliary tangent array tan1: cada vértice são 3 floats
		tV0 = faceIndex[i] * 3;
		tV1 = faceIndex[i + 1] * 3;
		tV2 = faceIndex[i + 2] * 3;

		float v0[] = { vertex[iV0], vertex[iV0 + 1], vertex[iV0 + 2] };
		float v1[] = { vertex[iV1], vertex[iV1 + 1], vertex[iV1 + 2] };
		float v2[] = { vertex[iV2], vertex[iV2 + 1], vertex[iV2 + 2] };

		float uv0[] = { texcoord[iV0], texcoord[iV0 + 1], 0.0f };
		float uv1[] = { texcoord[iV1], texcoord[iV1 + 1], 0.0f };
		float uv2[] = { texcoord[iV2], texcoord[iV2 + 1], 0.0f };

		float deltaPos1[] = { 0.0f, 0.0f, 0.0f };
		float deltaPos2[] = { 0.0f, 0.0f, 0.0f };
		subtract(v0, v1, deltaPos1);
		subtract(v0, v2, deltaPos2);

		float deltaUV1[] = { 0.0f, 0.0f, 0.0f};
		float deltaUV2[] = { 0.0f, 0.0f, 0.0f};
		subtract(uv0, uv1, deltaUV1);
		subtract(uv0, uv2, deltaUV2);

		float r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);

		float sdir[] = { (deltaUV2[1] * deltaPos1[0] - deltaUV1[1] * deltaPos2[0]) * r,
			(deltaUV2[1] * deltaPos1[1] - deltaUV1[1] * deltaPos2[1]) * r,
			(deltaUV2[1] * deltaPos1[2] - deltaUV1[1] * deltaPos2[2]) * r };

		float tdir[] = { (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0]) * r,
			(deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0]) * r,
			(deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0]) * r };

		add(&(tan1[tV0]), sdir, &(tan1[tV0]));
		add(&(tan1[tV1]), sdir, &(tan1[tV1]));
		add(&(tan1[tV2]), sdir, &(tan1[tV2]));

		add(&tan2[tV0], tdir, &tan2[tV0]);
		add(&tan2[tV1], tdir, &tan2[tV1]);
		add(&tan2[tV2], tdir, &tan2[tV2]);
	}

	for (int i = 0; i < vertexCount; i++) {
		float n[] = { normal[i * 4], normal[i * 4 + 1], normal[i * 4 + 2] };
		float t[] = { tan1[i * 3], tan1[i * 3 + 1], tan1[i * 3 + 2] };

		//printf("tan1 vector [%.2f, %.2f, %.2f]\n", t[0], t[1], t[2]);
		
		// Gram-Schmidt orthogonalize: T′ = T − (N · T)N
		constProduct(dotProduct(n, t), n, aux_vec);
		subtract(aux_vec, t, &tangent[i * 4]);
		normalize(&tangent[i * 4]);

		// Calculate handedness
		crossProduct(n, t, aux_vec);
		tangent[i * 4 + 3] = (dotProduct(aux_vec, &tan2[i * 3]) < 0.0f) ? -1.0f : 1.0f;
		//printf("tangent vector [%.2f, %.2f, %.2f, %.2f]\n", tangent[i * 4], tangent[i * 4 + 1], tangent[i * 4 + 2], tangent[i * 4 + 3]);
	}
}