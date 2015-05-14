#include "ShaderManager.h"
#include <conio.h>
#include "Movement.h"
#include "MovementSphere.h"


// gets called after the OpenGL window is prepared:
void ShaderManager::initCustomResources()
{

	//boost::filesystem::path full_path("./shaders/cylinder.vsh");
	//std::string apath = boost::filesystem::canonical(full_path).string();
	//_cprintf("path %s \n", apath.c_str() );

	string apath = ExePath();
	
	string vertexPath = apath + "\\shaders\\cylinder.vsh";
	string fragmentPath = apath + "\\shaders\\cylinder.fsh";
	string geometryPath = apath + "\\shaders\\cylinder.geo";

	string vertexPath1 = apath + "\\shaders\\normal_new.vsh";
	string fragmentPath1 = apath + "\\shaders\\normal_new.fsh";
	string geometryPath1 = apath + "\\shaders\\normal_new.geo";

	_cprintf("path %s \n", vertexPath.c_str() );

	torusShader = new ShaderProgram( vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str() );

	// set uniforms that don't change:
	initShaderUniform(torusShader);

	ballShader = new ShaderProgram( vertexPath1.c_str(), fragmentPath1.c_str(), geometryPath1.c_str() );
	initShaderUniform(ballShader);

	/*
	//init Perlin noise texture
	Noise perlin;
	perlin.make3DNoiseTexture();
	perlin.init3DNoiseTexture();
	//end of Perlin noise
	*/

	//initCupMap();

	glGenTextures(1, &begin_tex);
	glGenTextures(1, &end_tex);

	glGenTextures(1, &knn_tex);
	glGenTextures(1, &knn2_tex);
	glGenTextures(1, &knn3_tex);
	glGenTextures(1, &knn4_tex);

    openGLError(); // just to be sure
}



void ShaderManager::initShaderUniform(ShaderProgram *shader)
{
	shader->use();
    shader->setUniform( "uAmbientLight",      sceneAmbientLight );
    shader->setUniform( "uAmbientMaterial",   material.ambient );
    shader->setUniform( "uSpecularMaterial",  material.specular );
    shader->setUniform( "uShininessMaterial", material.shininess );
	shader->setUniform( "uDiffuseMaterial", material.diffuse );
    shader->setUniform( "uLightSource[0].color", lights[0].color );
    shader->setUniform( "uLightSource[1].color", lights[1].color );
    shader->setUniform( "uLightSource[2].color", lights[2].color );
}



void ShaderManager::drawScene()
{

	float aMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, aMatrix);
	viewMatrix = glm::make_mat4(aMatrix);

	//glGetFloatv(GL_PROJECTION_MATRIX, aMatrix);
	//projectionMatrix = glm::make_mat4(aMatrix);

	//glm::mat3 normalMatrix = glm::inverse( glm::transpose( glm::mat3(viewMatrix)));

	glm::vec3 bboxMax = glm::vec3(m_BBox.GetMaxX(), m_BBox.GetMaxY(), m_BBox.GetMaxZ());
	glm::vec3 bboxMin = glm::vec3(m_BBox.GetMinX(), m_BBox.GetMinY(), m_BBox.GetMinZ());

	switch (scene)
	{

	case 'a':

		break;

	case 'b':

		disableShaders();

		break;

	case 'c':
		
		initDynamicShaderUniform(torusShader);
		
		//use texture unit
		torusShader->setUniform("knn4_move", 26);
		torusShader->setUniform("knn3_move", 27);
		torusShader->setUniform("knn2_move", 28);
		torusShader->setUniform("knn_move", 29);

		//activate these texture units
		glActiveTexture(GL_TEXTURE0 + 26);
		glBindTexture(GL_TEXTURE_2D, knn4_tex);
		glActiveTexture(GL_TEXTURE0 + 27);
		glBindTexture(GL_TEXTURE_2D, knn3_tex);
		glActiveTexture(GL_TEXTURE0 + 28);
		glBindTexture(GL_TEXTURE_2D, knn2_tex);
		glActiveTexture(GL_TEXTURE0 + 29);
		glBindTexture(GL_TEXTURE_2D, knn_tex);

		torusShader->setUniform("begin_move", 30);
		torusShader->setUniform("end_move", 31);

		glActiveTexture(GL_TEXTURE0 + 30);
		glBindTexture(GL_TEXTURE_2D, begin_tex);
		glActiveTexture(GL_TEXTURE0 + 31);
		glBindTexture(GL_TEXTURE_2D, end_tex);



		break;

	case 'd':

		break;

	case 'e':

		break;

	case 'f':
		initDynamicShaderUniform(ballShader);

		//use texture unit 30 and 31
		ballShader->setUniform("begin_move", 30);
		ballShader->setUniform("end_move", 31);
		//activate these texture units
		glActiveTexture(GL_TEXTURE0 + 30);
		glBindTexture(GL_TEXTURE_2D, begin_tex);
		glActiveTexture(GL_TEXTURE0 + 31);
		glBindTexture(GL_TEXTURE_2D, end_tex);
		
		//ballShader->setUniform("size", (float)noMoves);
		//_cprintf("size uniform %d \n", noMoves );

		break;
	}

    openGLError();
}



void ShaderManager::setEye(float3d eye)
{
	//move the light away from the object to make sure it is not contained inside the object
	//lights[0].position = glm::vec3(eye[0]+20, eye[1]+20, eye[2]+20);
	lights[0].position = glm::vec3(eye[0]+0, eye[1]+0, eye[2]+0);
}



void ShaderManager::setShaderProgram(unsigned char shaderToUse)
{
	scene = shaderToUse;
}



void ShaderManager::switchProgram(ShaderProgram* program, const std::string &vertexShaderName, const std::string &fragmentShaderName)
{
	if (program != NULL) 
	{
		glUseProgram(0);
		delete program;
	}
	program = new ShaderProgram( vertexShaderName, fragmentShaderName );
	initShaderUniform(program);
	initDynamicShaderUniform(program);
}



void ShaderManager::switchProgram(ShaderProgram* program, 
								const std::string &vertexShaderName, 
								const std::string &fragmentShaderName,
								const std::string &geometryShaderName)
{
	if (program != NULL) 
	{
		glUseProgram(0);
		delete program;
	}
	program = new ShaderProgram( vertexShaderName, fragmentShaderName, geometryShaderName );
	initShaderUniform(program);
	initDynamicShaderUniform(program);
}



void ShaderManager::initDynamicShaderUniform(ShaderProgram* shader)
{
	glm::vec4 lightPos;
	shader->use();
	// set light parameters:
	shader->setUniform( "numberOfLights", 1);
	lightPos = viewMatrix * glm::vec4( lights[0].position, 1.0);
	shader->setUniform( "uLightSource[0].position", glm::vec3(lightPos) );
	lightPos = viewMatrix * glm::vec4( lights[1].position, 1.0);
	shader->setUniform( "uLightSource[1].position", glm::vec3(lightPos) );
	lightPos = viewMatrix * glm::vec4( lights[2].position, 1.0);
	shader->setUniform( "uLightSource[2].position", glm::vec3(lightPos) );
}



// gets called at program shutdown:
// don't forget to delete all your resources!
void ShaderManager::deleteCustomResources()
{
    delete cylinderShader;
	delete ballShader;
	delete torusShader;

	glDeleteTextures(1, &knn_tex);
	glDeleteTextures(1, &knn2_tex);
	glDeleteTextures(1, &knn3_tex);
	glDeleteTextures(1, &knn4_tex);

	glDeleteTextures(1, &begin_tex);
	glDeleteTextures(1, &end_tex);
	
}



//
// Can be useful if your hardware supports ARB_debug_output. If you have compile problems with this,
// just comment it out.
//
// NVidia warnings: "Program/shader state performance warning: Fragment Shader is going to be recompiled
// because the shader key based on GL state mismatches." that only appear once can be ignored ;-)
//
void ShaderManager::debugCallback(GLenum source, GLenum type, GLuint id,
								GLenum severity, GLsizei length,
								const GLchar  *message, GLvoid *userParam)
{

	if (userParam != NULL)
	{
		std::cout << "userParam not null";
	} 
	else
	{
		std::cout << "userParam null";
	}
	std::cout << length << id;

     cout << "Note: ";
	 _cprintf("Note: ");
     if (source == GL_DEBUG_SOURCE_API_ARB)
            _cprintf("OpenGL");//cout << "OpenGL";
     else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
            _cprintf("your OS");//cout << "your OS";
     else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
            _cprintf("the Shader Compiler");//cout << "the Shader Compiler";
     else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
            _cprintf("a third party component");//cout << "a third party component";
     else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
            _cprintf("your application");//cout << "your application";
     else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
            _cprintf("someone");//cout << "someone";

     _cprintf(" reported a problem - it's a");//cout << " reported a problem - it's a";
     if (type == GL_DEBUG_TYPE_ERROR_ARB)
            _cprintf("n error");//cout << "n error";
     else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
            _cprintf(" deprecated behavior");//cout << " deprecated behavior";
     else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
            _cprintf("n undefined behavior");//cout << "n undefined behavior";
     else if(type == GL_DEBUG_TYPE_PORTABILITY_ARB)
            _cprintf(" portability issue");//cout << " portability issue";
     else if(type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
            _cprintf(" performance issue");//cout << " performance issue";
     else if(type == GL_DEBUG_TYPE_OTHER_ARB)
            _cprintf(" something");//cout << " something";

     _cprintf(". You ");//cout << ". You ";

     if(severity == GL_DEBUG_SEVERITY_HIGH_ARB)
            _cprintf("should fix it!");//cout << "should fix it before handing in the exercise!";
     else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
            _cprintf("should look at it");//cout << "should look at it";
     else if(severity == GL_DEBUG_SEVERITY_LOW_ARB)
            _cprintf("might look at it \n");//cout << "might look at it";

     cout << endl;
     cout << "The massage was: " << message << endl << endl;
	 _cprintf("The massage was: %s \n", std::string(message).c_str());
}



void ShaderManager::initializeOpenGL() 
{
    if (GLEW_ARB_debug_output) {
        cout << "GL_ARB_DEBUG_OUTPUT is supported, register callback" << endl;

		_cprintf("GL_ARB_DEBUG_OUTPUT is supported, register callback \n");

        glDebugMessageCallbackARB( ShaderManager::debugCallback, NULL);
    }
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glEnable( GL_DEPTH_TEST );

    initCustomResources();
}



///*
TextureData* ShaderManager::readPPMFile( const std::string &fileName)
{
    std::ifstream fileStream(fileName.c_str(), std::ifstream::in);

    if (!fileStream.good()) {
        cerr << "could not open file " << fileName << endl;
        return NULL;
    }

    TextureData *texData = new TextureData;

    int width, height, maxValue;

    std::string line;
    fileStream >> line; // ignore the header

    // note: we assume no comments in this file!
    // a general ppm loader has to be more clever!

    fileStream >> width;
    fileStream >> height;
    fileStream >> maxValue; // we will ignore this and assume the value range is 0..255

    unsigned char *data = new unsigned char[width*height*3]; // we assume RGB

    int pos = 0;
    while (fileStream.good() && pos < (width*height*3)) {
        int i;
        fileStream >> i;
        data[pos] = (unsigned char) i;
        pos++;
    }
    fileStream.close();

    texData->setData(data); // data will get deleted by the TextureData destructor!
    texData->setDepth(1);   // 2D so, depth is 1
    texData->setHeight(height);
    texData->setWidth(width);
    texData->setFormat(GL_RGB);
    texData->setType(GL_UNSIGNED_BYTE); // == unsigned char

    return texData;
}
//*/



///*
void ShaderManager::initCupMap()
{

	//read ppm 6 cube images
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_positive_x.ppm"));
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_negative_x.ppm"));
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_positive_y.ppm"));
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_negative_y.ppm"));
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_positive_z.ppm"));
	cubeMap.push_back(readPPMFile("../shaders/cubeMap/nvlobby_new_negative_z.ppm"));

	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &cubemap_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

	for (int i=0; i<6; i++) {

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, cubeMap[i]->getWidth(),
			cubeMap[i]->getHeight(),0, cubeMap[i]->getFormat(), cubeMap[i]->getType(), cubeMap[i]->getData());

	}
}
//*/



void ShaderManager::build_tool_path_texture(GLfloat* begin_list, GLfloat* end_list, 
										  GLuint & tex_begin, GLuint & tex_end)
{

	//do not need to activate texture
	//glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_2D, tex_begin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized move list
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGB, GL_FLOAT, begin_list);


	//glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, tex_end);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, end_list);

}



void ShaderManager::initMoveLists()
{
	mwMachSimVerifier::MoveSimulationList simulationMoveList;
	verifier->GetMoveSimulationList(simulationMoveList);

	if (m_iActualSimulatedPos == simulationMoveList.size())
	{
		return;
	}

	GLfloat* beginList = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*3];
	memset(beginList, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*3);
	GLfloat* endList = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(endList, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);

	_cprintf( "#moves: %d \n", simulationMoveList.size() );

	// Iterate over all moves and calculate normal
	for(size_t i = 0; i < simulationMoveList.size(); ++i)
	{
		// Get tool path positions
		float3d beginMove = simulationMoveList[i].beginFrame.m_origin;
		float3d endMove = simulationMoveList[i].endFrame.m_origin;

		// Get tool axis
		float3d beginAxis = simulationMoveList[i].beginFrame.getAxis();
		float3d endAxis = simulationMoveList[i].endFrame.getAxis();
		// Get radius of current tool
		float radius = simulationMoveList[i].toolRadius;

		// Move toolpath to center of tool
		beginMove += beginAxis * radius;
		endMove += endAxis * radius;

		// Calculate the direction vector
		float3d moveVec = endMove - beginMove;
		float length = ~moveVec;
		if (beginMove != endMove) moveVec.Normalize();

		beginList[i*3] = beginMove.x();
		beginList[i*3+1] = beginMove.y();
		beginList[i*3+2] = beginMove.z();

		endList[i*4] = moveVec.x();
		endList[i*4+1] = moveVec.y();
		endList[i*4+2] = moveVec.z();
		endList[i*4+3] = length;
		
	}

	build_tool_path_texture(beginList, endList, begin_tex, end_tex);

	delete [] beginList;
	delete [] endList;

	m_iActualSimulatedPos = simulationMoveList.size();

}



void ShaderManager::initCylinderMoveLists()
{
	KnnMapType knn_Map;
	mwMachSimVerifier::MoveSimulationList simulationMoveList;
	verifier->GetMoveSimulationList(simulationMoveList);

	if (m_iActualSimulatedPos == simulationMoveList.size())
	{
		return;
	}

	///*
	//8x8x8 is the best resolution in terms of speed
	std::vector< std::vector<ShapeSphere*> > voxelList = BuildVoxelMoveList(simulationMoveList, 8, 8, 8);
	_cprintf("build voxel move list successfully \n");
	BuildVoxelKnn(voxelList, knn_Map, simulationMoveList.size());
	//*/

	GLfloat* beginList = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(beginList, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);
	GLfloat* endList = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(endList, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);

	//for knn
	GLfloat* knn = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(knn, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);
	GLfloat* knn2 = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(knn2, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);

	GLfloat* knn3 = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(knn3, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);
	GLfloat* knn4 = new GLfloat[GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4];
	memset(knn4, 0, GridMapping::GLOBAL_MOVE*GridMapping::GLOBAL_MOVE*4);

	// Iterate over all moves and calculate normal
	for(size_t i = 0; i < simulationMoveList.size(); ++i)
	{
		// Get tool path positions
		float3d beginMove = simulationMoveList[i].beginFrame.m_origin;
		float3d endMove = simulationMoveList[i].endFrame.m_origin;

		// Get tool axis
		float3d beginAxis = simulationMoveList[i].beginFrame.getAxis();
		float3d endAxis = simulationMoveList[i].endFrame.getAxis();
		// Get radius of current tool
		float radius = simulationMoveList[i].toolRadius;
		float toolHeight = simulationMoveList[i].toolHeight;

		// Move toolpath to center of tool
		//beginMove += beginAxis * radius;
		//endMove += endAxis * radius;

		// Calculate the direction vector
		//float3d moveVec = endMove - beginMove;
		//float length = ~moveVec;
		//if (beginMove != endMove) moveVec.Normalize();

		beginList[i*4] = beginMove.x();
		beginList[i*4+1] = beginMove.y();
		beginList[i*4+2] = beginMove.z();
		beginList[i*4+3] = toolHeight;

		endList[i*4] = endMove.x();
		endList[i*4+1] = endMove.y();
		endList[i*4+2] = endMove.z();
		endList[i*4+3] = radius;

		std::vector<ShapeSphere::NeighborItem> neighbors = knn_Map[i];
		neighbors.size() < 1 ? knn[i*4] = static_cast<GLfloat>(i) : knn[i*4] = static_cast<GLfloat>(neighbors[0].id);
		neighbors.size() < 2 ? knn[i*4+1] = static_cast<GLfloat>(i) : knn[i*4+1] = static_cast<GLfloat>(neighbors[1].id);
		neighbors.size() < 3 ? knn[i*4+2] = static_cast<GLfloat>(i) : knn[i*4+2] = static_cast<GLfloat>(neighbors[2].id);
		neighbors.size() < 4 ? knn[i*4+3] = static_cast<GLfloat>(i) : knn[i*4+3] = static_cast<GLfloat>(neighbors[3].id);

		neighbors.size() < 5 ? knn2[i*4] = static_cast<GLfloat>(i) : knn2[i*4] = static_cast<GLfloat>(neighbors[4].id);
		neighbors.size() < 6 ? knn2[i*4+1] = static_cast<GLfloat>(i) : knn2[i*4+1] = static_cast<GLfloat>(neighbors[5].id);
		neighbors.size() < 7 ? knn2[i*4+2] = static_cast<GLfloat>(i) : knn2[i*4+2] = static_cast<GLfloat>(neighbors[6].id);
		neighbors.size() < 8 ? knn2[i*4+3] = static_cast<GLfloat>(i) : knn2[i*4+3] = static_cast<GLfloat>(neighbors[7].id);

		neighbors.size() < 9 ? knn3[i*4] = static_cast<GLfloat>(i) : knn3[i*4] = static_cast<GLfloat>(neighbors[8].id);
		neighbors.size() < 10 ? knn3[i*4+1] = static_cast<GLfloat>(i) : knn3[i*4+1] = static_cast<GLfloat>(neighbors[9].id);
		neighbors.size() < 11 ? knn3[i*4+2] = static_cast<GLfloat>(i) : knn3[i*4+2] = static_cast<GLfloat>(neighbors[10].id);
		neighbors.size() < 12 ? knn3[i*4+3] = static_cast<GLfloat>(i) : knn3[i*4+3] = static_cast<GLfloat>(neighbors[11].id);

		neighbors.size() < 13 ? knn4[i*4] = static_cast<GLfloat>(i) : knn4[i*4] = static_cast<GLfloat>(neighbors[12].id);
		neighbors.size() < 14 ? knn4[i*4+1] = static_cast<GLfloat>(i) : knn4[i*4+1] = static_cast<GLfloat>(neighbors[13].id);
		neighbors.size() < 15 ? knn4[i*4+2] = static_cast<GLfloat>(i) : knn4[i*4+2] = static_cast<GLfloat>(neighbors[14].id);
		neighbors.size() < 16 ? knn4[i*4+3] = static_cast<GLfloat>(i) : knn4[i*4+3] = static_cast<GLfloat>(neighbors[15].id);

	}

	upload_cylinder_texture(beginList, endList, knn, knn2, knn3, knn4);

	delete [] beginList;
	delete [] endList;
	delete [] knn;
	delete [] knn2;
	delete [] knn3;
	delete [] knn4;

	m_iActualSimulatedPos = simulationMoveList.size();

}



void ShaderManager::upload_cylinder_texture(GLfloat* beginMove, GLfloat* endMove, GLfloat* knn, GLfloat* knn2, GLfloat* knn3, GLfloat* knn4)
{
	//do not need to activate texture
	//glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_2D, begin_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized move list
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, beginMove);


	//glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, end_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, endMove);


	//glActiveTexture(GL_TEXTURE29);
	glBindTexture(GL_TEXTURE_2D, knn_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, knn);


	//glActiveTexture(GL_TEXTURE28);
	glBindTexture(GL_TEXTURE_2D, knn2_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, knn2);


	glBindTexture(GL_TEXTURE_2D, knn3_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, knn3);


	glBindTexture(GL_TEXTURE_2D, knn4_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//use non-normalized, the alpha component is for radius
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GridMapping::GLOBAL_MOVE, GridMapping::GLOBAL_MOVE
		, 0, GL_RGBA, GL_FLOAT, knn4);

}



std::vector< std::vector<ShapeSphere*> > ShaderManager::BuildVoxelMoveList(const mwMachSimVerifier::MoveSimulationList & simulationMoveList, 
																	  float sizeX, float sizeY, float sizeZ)
{
	std::vector< std::vector<ShapeSphere*> > childrenList(static_cast<size_t>(sizeX*sizeY*sizeZ));
	m_SizeX = sizeX;
	m_SizeY = sizeY;
	m_SizeZ = sizeZ;

	m_BBox = Shape::BBox(simulationMoveList[0].beginFrame.getOrigin(), simulationMoveList[0].endFrame.getOrigin());
	for (size_t i = 1; i < simulationMoveList.size(); ++i)
	{
		m_BBox.EnlargeForPoint(simulationMoveList[i].beginFrame.getOrigin());
		m_BBox.EnlargeForPoint(simulationMoveList[i].endFrame.getOrigin());
	}

	float3d sampleMove = simulationMoveList[simulationMoveList.size()-1].endFrame.getOrigin() 
				- simulationMoveList[simulationMoveList.size()-1].beginFrame.getOrigin();

	m_BBox.Inflate(sqrt(sampleMove.Length2())/4);

	m_ScaleX = m_SizeX / m_BBox.GetWidth();
	m_ScaleY = m_SizeY / m_BBox.GetHeight();
	m_ScaleZ = m_SizeZ / m_BBox.GetDepth();

	_cprintf("children list size %d \n", childrenList.size());
	float heavyValue = 100000000.0f;

	//int i = 25384;
	for (size_t i = 0; i < simulationMoveList.size(); ++i)
	{
		float start_d, start_h, start_w, end_d, end_h, end_w;
		
		float3d start = simulationMoveList[i].beginFrame.getOrigin();
		float3d end = simulationMoveList[i].endFrame.getOrigin();

		get_cell_id(start, start_d, start_h, start_w);
		get_cell_id(end, end_d, end_h, end_w);

		//_cprintf("start (%f, %f, %f), end (%f, %f, %f) \n", start_w, start_h, start_d, end_w, end_h, end_d);

		float3d moveDirection = end - start;
		moveDirection.Normalize();

		int stepX = moveDirection[0] > 0 ? 1 : -1;
		int stepY = moveDirection[1] > 0 ? 1 : -1;
		int stepZ = moveDirection[2] > 0 ? 1 : -1;

		//_cprintf("step x %d, step y %d, step z %d \n", stepX, stepY, stepZ);

		float upperX = (start_w + 1)/m_ScaleX + m_BBox.GetMinX();
		float upperY = (start_h + 1)/m_ScaleY + m_BBox.GetMinY();
		float upperZ = (start_d + 1)/m_ScaleZ + m_BBox.GetMinZ();

		float tmaxX = fabs( (upperX - start[0])/moveDirection[0] );
		float tmaxY = fabs( (upperY - start[1])/moveDirection[1] );
		float tmaxZ = fabs( (upperZ - start[2])/moveDirection[2] );

		//_cprintf("tmax x %f, tmax y %f, tmax z %f \n", tmaxX, tmaxY, tmaxZ);

		//float deltaX = m_ScaleX / moveDirection[0];
		//float deltaY = m_ScaleY / moveDirection[1];
		//float deltaZ = m_ScaleZ / moveDirection[2];

		float deltaX = fabs( m_BBox.GetWidth() / ( m_SizeX*moveDirection[0] ) );
		float deltaY = fabs( m_BBox.GetHeight() / ( m_SizeY*moveDirection[1] ) );
		float deltaZ = fabs( m_BBox.GetDepth() / ( m_SizeZ*moveDirection[2] ) );

		//_cprintf("delta x %f, delta y %f, delta z %f \n", deltaX, deltaY, deltaZ);

		if (start_d == end_d && start_h == end_h && start_w == end_w)
		{
			size_t listId = GetListId(start_d, start_h, start_w);

			//_cprintf("start (%f, %f, %f), end (%f, %f, %f) \n", start_w, start_h, start_d, end_w, end_h, end_d);
			//_cprintf("move id %d, list id %d \n", i, listId);

			childrenList[listId].push_back(new MovementSphere(simulationMoveList[i].beginFrame.m_origin, 
				simulationMoveList[i].endFrame.m_origin, simulationMoveList[i].beginFrame.getAxis(), 
				simulationMoveList[i].endFrame.getAxis(), 0, i));
		} 
		else
		{
			do
			{

				size_t listId = GetListId(start_d, start_h, start_w);

				//_cprintf("start (%f, %f, %f), end (%f, %f, %f) \n", start_w, start_h, start_d, end_w, end_h, end_d);
				//_cprintf("move id %d, list id %d \n", i, listId);

				childrenList[listId].push_back(new MovementSphere(simulationMoveList[i].beginFrame.m_origin, 
					simulationMoveList[i].endFrame.m_origin, simulationMoveList[i].beginFrame.getAxis(), 
					simulationMoveList[i].endFrame.getAxis(), 0, i));

				if (start_w == end_w) tmaxX += heavyValue;
				if (start_h == end_h) tmaxY += heavyValue;
				if (start_d == end_d) tmaxZ += heavyValue;

				if (tmaxX < tmaxY)
				{
					if (tmaxX < tmaxZ)
					{
						start_w += stepX;
						tmaxX += deltaX;
					} 
					else
					{
						start_d += stepZ;
						tmaxZ += deltaZ;
					}
				} 
				else
				{
					if (tmaxY < tmaxZ)
					{
						start_h += stepY;
						tmaxY += deltaY;
					} 
					else
					{
						start_d += stepZ;
						tmaxZ += deltaZ;
					}
				}
				//_cprintf("start (%f, %f, %f) \n", start_w, start_h, start_d);
				//_cprintf("tmax x %f, tmax y %f, tmax z %f \n", tmaxX, tmaxY, tmaxZ);
			}
			while (start_d != end_d || start_h != end_h || start_w != end_w);

			size_t listId = GetListId(start_d, start_h, start_w);

			//_cprintf("start (%f, %f, %f), end (%f, %f, %f) \n", start_w, start_h, start_d, end_w, end_h, end_d);
			//_cprintf("move id %d, list id %d \n", i, listId);

			childrenList[listId].push_back(new MovementSphere(simulationMoveList[i].beginFrame.m_origin, 
				simulationMoveList[i].endFrame.m_origin, simulationMoveList[i].beginFrame.getAxis(), 
				simulationMoveList[i].endFrame.getAxis(), 0, i));
		}
	} 

	return childrenList;
}



void ShaderManager::get_cell_id(const float3d & corner, float & depth_id, float & height_id, float & width_id)
{
	depth_id = floor( m_ScaleZ*(corner[2] - m_BBox.GetMinZ()) );
	height_id = floor( m_ScaleY*(corner[1] - m_BBox.GetMinY()) );
	width_id = floor( m_ScaleX *(corner[0] - m_BBox.GetMinX()) );	
}



size_t ShaderManager::GetListId(const float & depth_id, const float & height_id, const float & width_id)
{
	float listId = depth_id*m_SizeY*m_SizeX + height_id*m_SizeX + width_id;
	return static_cast<size_t>(listId);
}



void ShaderManager::BuildVoxelKnn(std::vector< std::vector<ShapeSphere*> > & voxelListRedundant, KnnMapType & totalMap, size_t noMoves)
{

	std::vector< std::vector<ShapeSphere*> > voxelList;

	for (int i = 0; i < voxelListRedundant.size(); ++i)
	{
		if (voxelListRedundant[i].size() > 0)
		{
			voxelList.push_back(voxelListRedundant[i]);
		}
	}

	std::vector<KnnMapType> knn_Maps(voxelList.size());
	//std::vector<BVH*> bvhList(voxelList.size());
	std::vector<BVHSphere*> bvhList(voxelList.size());

	#pragma omp parallel for
	for (int i = 0; i < voxelList.size(); ++i)
	{
		bvhList[i] = new BVHSphere(voxelList[i]);
		//bvhList[i]->draw();
		bvhList[i]->buildKnnExactMap(knn_Maps[i], NUMBER_OF_NEIGHBORS, bvhList[i]);
	}

	_cprintf("knn list done \n");

	//merge the list
	for (int i = 0; i < noMoves; ++i)
	{
		bool isFirst = true;
		std::vector<ShapeSphere::NeighborItem> finalList;

		for (int j = 0; j < voxelList.size(); ++j)
		{
			//KnnMapType knnMap = knn_Maps[j];
			if ( knn_Maps[j].count(i) > 0 )
			{
				if (isFirst)
				{
					finalList = knn_Maps[j][i];
					isFirst = false;
				}else
				{
					std::vector<ShapeSphere::NeighborItem> intermediate(NUMBER_OF_NEIGHBORS);
					mergeKnnList(finalList, knn_Maps[j][i], intermediate, NUMBER_OF_NEIGHBORS);
					finalList = intermediate;
				}
			}
		}

		totalMap[i] = finalList;
		//_cprintf("merge list number %d, list size %d \n", i, finalList.size());
	}

	_cprintf("merge list done \n");

	#pragma omp parallel for
	for (int i = 0; i < voxelList.size(); ++i)
	{
		bvhList[i]->freeNode();
		delete bvhList[i];
	}

}



void ShaderManager::mergeKnnList(const std::vector<ShapeSphere::NeighborItem> & list1, const std::vector<ShapeSphere::NeighborItem> & list2, 
							   std::vector<ShapeSphere::NeighborItem> & resultList, const size_t & noNeighbors)
{
	resultList.clear();
	int index1 = 0;
	int index2 = 0;

	while ( resultList.size() < noNeighbors )
	{
		if (index1 < list1.size() && index2 < list2.size())
		{
			if (list1[index1].ranking < list2[index2].ranking)
			{
				resultList.push_back(list1[index1]);
				index1++;
			} 
			else if (list2[index2].ranking < list1[index1].ranking)
			{
				resultList.push_back(list2[index2]);
				index2++;
			}
			else// equal
			{

				if (list1[index1].id == list2[index2].id)
				{
					resultList.push_back(list1[index1]);
					index1++;
					index2++;
				}else//either one is ok
				{
					resultList.push_back(list1[index1]);
					index1++;
				}

			}
		} 
		else 
		{
			if (index1 < list1.size())
			{
				resultList.push_back(list1[index1]);
				index1++;
			} 
			else if (index2 < list2.size())
			{
				resultList.push_back(list2[index2]);
				index2++;
			}
			else
			{
				break;
			}
		}

	}

}



string ShaderManager::ExePath() 
{
	char buffer[MAX_PATH];
	GetModuleFileNameA( NULL, buffer, MAX_PATH );
	string::size_type pos = string( buffer ).find_last_of( "\\/" );
	return string( buffer ).substr( 0, pos);
}

