#include "gameObj.h"

gameObj::gameObj()
{
	
}

gameObj::gameObj(string modelPath,Shader *shader) {}

void gameObj::init(string modelPath, Shader* shader,bool centre){
	myShader = shader;
	if (objLoader.loadModel(&modelPath[0], model))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		model.calcCentrePoint();
		if (centre) {
			model.centreOnZero();
		}
		model.calcVertNormalsUsingOctree();

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		model.initDrawElements();
		model.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	minx = miny = minz = maxx = maxy = maxz = 0;

	model.calcBoundingBox(minx, miny, minz, maxx, maxy, maxz);

	objectRotationMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	modelViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	pos = glm::vec3(0.0);

}

void gameObj::scale(float x, float y, float z)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(x, y, z));
}

void gameObj::updatePos(glm::vec3 new_pos)
{	
	pos = new_pos;
	modelMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void gameObj::updatePos(float xinc, float yinc, float zinc, float moveXinc, float moveYinc, float moveZinc, float deltaTime)
{
	objectRotationMatrix = glm::rotate(objectRotationMatrix, xinc * deltaTime, glm::vec3(1, 0, 0));
	objectRotationMatrix = glm::rotate(objectRotationMatrix, yinc * deltaTime, glm::vec3(0, 1, 0));
	objectRotationMatrix = glm::rotate(objectRotationMatrix, zinc * deltaTime, glm::vec3(0, 0, 1));

	pos.x += objectRotationMatrix[2][0] * moveZinc * deltaTime;
	pos.y += objectRotationMatrix[2][1] * moveZinc * deltaTime;
	pos.z += objectRotationMatrix[2][2] * moveZinc * deltaTime;

	pos.x += objectRotationMatrix[0][0] * moveXinc * deltaTime;
	pos.y += objectRotationMatrix[0][1] * moveXinc * deltaTime;
	pos.z += objectRotationMatrix[0][2] * moveXinc * deltaTime;

	pos.x += objectRotationMatrix[1][0] * moveYinc * deltaTime;
	pos.y += objectRotationMatrix[1][1] * moveYinc * deltaTime;
	pos.z += objectRotationMatrix[1][2] * moveYinc * deltaTime;

	modelMatrix = glm::translate(glm::mat4(1.0f), pos);
	//std::cout << deltaTime << std::endl;
}

void gameObj::draw(glm::mat4 viewingMatrix)
{
	glUseProgram(myShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	modelViewMatrix = viewingMatrix * modelMatrix * objectRotationMatrix;
	glm::mat4 temp = modelMatrix * objectRotationMatrix;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));


	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelMatrix"), 1, GL_FALSE, &temp[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);


	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, &LightPos[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, &Light_Specular[0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "Cone_Light_Ambient_And_Diffuse"), 1, &Cone_Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "Cone_Light_Specular"), 1, &Cone_Light_Specular[0]);


	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, &Material_Ambient[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, &Material_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, &Material_Specular[0]);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);


	model.drawElementsUsingVBO(myShader);
	//model.drawOctreeLeaves(myShader);
	//cout << pos.x << endl;
}

void gameObj::debug(glm::mat4 viewingMatrix)
{
	glUseProgram(myShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	modelViewMatrix = viewingMatrix * modelMatrix * objectRotationMatrix;
	glm::mat4 temp = modelMatrix * objectRotationMatrix;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));


	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelMatrix"), 1, GL_FALSE, &temp[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);


	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, &LightPos[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, &Light_Specular[0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, &Material_Ambient[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, &Material_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, &Material_Specular[0]);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);


	model.drawElementsUsingVBO(myShader);
	model.drawOctreeLeaves(myShader);
	//cout << pos.x << endl;
}

bool gameObj::testCollision(glm::vec3 testPoint)
{
	return false;
}

//glm::vec3* gameObj::testCollision(std::vector<glm::vec3> testPoints)
//{	
//	for (glm::vec3 point : testPoints) {
//		if (model.checkPointOctree(point))
//		{
//		
//			return &point;
//		}
//	}
//	return NULL;
//}

void gameObj::rotate(float degree, glm::vec3 axis) {
	objectRotationMatrix = glm::rotate(objectRotationMatrix, glm::radians(degree), axis);
}

