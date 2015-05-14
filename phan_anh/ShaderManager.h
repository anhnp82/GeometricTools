#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#include "Shape.h"
#include "ShapeSphere.h"
//#include "boost/filesystem.hpp" 

#include "ShaderProgram.h"
#include "ArrayBuffer.h"
#include "VertexArrayObject.h"
#include "Tools.h"
#include "TextureData.h"

#pragma warning (disable: 4201)
#include "glm-0.9.3.4/glm/glm.hpp"
#include "glm-0.9.3.4/glm/ext.hpp"
#pragma warning (default: 4201)

#include <limits>
using namespace std;

#include "../VasDefines.hpp"
#include "mwMachSimVerifier.hpp"
#include "Noise.h"
#include "GridMapping.h"

#include "BVH.h"
#include "BVHSphere.h"


#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif



struct LightSourceParameters
{
    glm::vec3 color;
    glm::vec3 position;
};

struct MaterialParameters
{
    glm::vec3 ambient;  // alpha_a in our slides
    glm::vec3 diffuse;  // alpha_d
    glm::vec3 specular; // alpha_sp
    float shininess;    // exponent 's' in our slides
};

class ShaderManager
{
private:

	//typedef unordered_map< size_t, std::vector<size_t> > KnnMapType;
	typedef ShapeSphere::KnnMapType KnnMapType;
	//typedef boost::unordered_map< size_t, std::vector<NeighborItem> > KnnMapType;

	LightSourceParameters lights[3];
	glm::vec3 sceneAmbientLight; // C_a from the slides

	MaterialParameters material;

	ShaderProgram *torusShader;
	ShaderProgram *cylinderShader;
	ShaderProgram *ballShader;

	glm::mat4 viewMatrix;       // for the camera

	unsigned char scene;

	std::vector<TextureData*> cubeMap;
	GLuint cubemap_texture;

	mwMachSimVerifier* verifier;
	size_t m_iActualSimulatedPos;

	GLuint begin_tex;
	GLuint end_tex;
	GLuint knn_tex;
	GLuint knn2_tex;
	GLuint knn3_tex;
	GLuint knn4_tex;

	Shape::BBox m_BBox;
	float m_SizeX, m_SizeY, m_SizeZ;
	float m_ScaleX, m_ScaleY, m_ScaleZ;

	std::vector< std::vector<ShapeSphere*> > BuildVoxelMoveList(const mwMachSimVerifier::MoveSimulationList & simulationMoveList, 
		float sizeX, float sizeY, float sizeZ);

	void get_cell_id(const float3d & corner, float & depth_id, float & height_id, float & width_id);

	size_t GetListId(const float & depth_id, const float & height_id, const float & width_id);

	/*
	* some items of the voxelList are swapped
	*/
	void BuildVoxelKnn(std::vector< std::vector<ShapeSphere*> > & voxelList, KnnMapType & totalMap, size_t noMoves);

	/*
	* merge 2 ascending order lists
	*/
	void mergeKnnList(const std::vector<ShapeSphere::NeighborItem> & list1, const std::vector<ShapeSphere::NeighborItem> & list2, 
					  std::vector<ShapeSphere::NeighborItem> & resultList, const size_t & noNeighbors);

	//////////////////////////////////////////////////////////////////////////
	


	string ExePath();

	void upload_cylinder_texture(GLfloat* beginMove, GLfloat* endMove, GLfloat* knn, GLfloat* knn2, GLfloat* knn3, GLfloat* knn4);

	void build_tool_path_texture(GLfloat* begin_list, GLfloat* end_list, 
		GLuint & tex_begin, GLuint & tex_end);

	static void CALLBACK debugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar  *message, GLvoid *userParam);



	/*
	* init constant uniforms for each shaders
	*/
	void initShaderUniform(ShaderProgram* shader);

	/*
	* init dynamic uniforms for each shaders
	*/
	void initDynamicShaderUniform(ShaderProgram* shader);

	void switchProgram(ShaderProgram* program, 
		const std::string &vertexShaderName, 
		const std::string &fragmentShaderName,
		const std::string &geometryShaderName);

	void switchProgram(ShaderProgram* program, 
		const std::string &vertexShaderName, 
		const std::string &fragmentShaderName);

	void initCupMap();

	/*
	* Given a filename of an ASCII ppm image file (without any comments) this will
	* read in the values and return an object with the raw image data and some basic information
	* (like the image dimensions).
	*
	* The caller is responsible to delete the returned data.
	*/
	TextureData *readPPMFile( const std::string &fileName);

	void initCustomResources();

	

	void deleteCustomResources();

public:

	static const int NUMBER_OF_NEIGHBORS = 16;

	ShaderManager(): m_iActualSimulatedPos(0)
	{
		lights[0].color		= glm::vec3(1.0,1.0,1.0);
		lights[1].color		= glm::vec3(0.5,0.5,0.5);
		lights[2].color		= glm::vec3(0.5,0.5,0.5);

		lights[0].position	= glm::vec3( 4.0, 6.0, -2.0);
		lights[1].position	= glm::vec3(-4.0, 6.0,  3.0);
		lights[2].position	= glm::vec3( 0.0, 6.0,  4.0);

		//no ambient
		material.ambient	= glm::vec3(0.0, 0.0, 0.0);
		//white object
		material.diffuse	= glm::vec3(1.0, 1.0, 1.0);
		material.specular	= glm::vec3(1.0, 1.0, 1.0);
		material.shininess	= 128.0f;

		//no ambient
		sceneAmbientLight  = glm::vec3( 0.0, 0.0, 0.0 );

		//default scene
		scene = 'b';

	}

	~ShaderManager()
	{
		deleteCustomResources();
	}


	void setEye(float3d eye);


	void drawScene();

	void disableShaders()
	{
		glUseProgram(0);
	}

	/*
	* switch between different shaders
	*/
	void setShaderProgram(unsigned char shaderToUse);

	void initializeOpenGL() ;



	void setVerifier(mwMachSimVerifier* aVerifier)
	{
		verifier = aVerifier;
	}

	void initMoveLists();
	void initCylinderMoveLists();

};
