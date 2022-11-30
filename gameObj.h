#pragma once
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "shaders\Shader.h"

#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
class gameObj
{
private:
	OBJLoader objLoader;

public:
	double minx , miny, minz, maxx, maxy, maxz;
	ThreeDModel model;
	glm::mat4 modelMatrix;
	glm::mat4 objectRotationMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 modelViewMatrix;
	glm::vec3 pos;
	Shader *myShader;

	//Material properties
	glm::vec4 Material_Ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 Material_Diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f );
	glm::vec4 Material_Specular = glm::vec4(0.04f,0.04f,0.04f,1.0f);
	float Material_Shininess = 10;

	//Light Properties
	glm::vec4 Light_Ambient_And_Diffuse = glm::vec4( 1.0f,0.5f,0.4f, 1.0f );
	glm::vec4 Light_Specular = glm::vec4( 1.0f,0.5f,0.4f,1.0f );
	glm::vec4 LightPos = glm::vec4( 1.0f, 0.3f, -1.0f, 0.0f );
	glm::vec4 Cone_Light_Ambient_And_Diffuse = glm::vec4( 0.8f,0.8f,0.9f, 1.0f );
	glm::vec4 Cone_Light_Specular = glm::vec4(0.8f, 0.8f, 0.9f,1.0f );


	gameObj();
	gameObj(string modelName, Shader* shader);
	void init(string modelPath, Shader* shader,bool center);
	void scale(float x, float y, float z);
	void updatePos(glm::vec3 new_pos);
	void updatePos(float xinc, float yinc, float zinc, float moveXinc, float moveYinc, float moveZinc, float deltaTime);
	void draw(glm::mat4 viewingMatrix);
	void debug(glm::mat4 viewingMatrix);
	bool testCollision(glm::vec3 testPoint);
	glm::vec3* testCollision(std::vector<glm::vec3> testPoints);
	void rotate(float degree, glm::vec3 axis);
	
};

