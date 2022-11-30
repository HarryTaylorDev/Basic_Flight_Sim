#include "plane.h"

plane::plane(){}

void plane::init(string modelPath, string modelPathCrashed, Shader *shader)
{
	gameObj::init(modelPathCrashed, shader,true);

	gravity = 9.8f;
	gravVelocity = 0.0f;
	velocity = 10.0f;
	maxVelocity = 400.0;
	speed = 4.0f;   // change to better value at later date
	crashed = false;
	landed = true;
	pos = glm::vec3(0.0, 900, 0.0);
	defualtPlane = model;
	pitchVelocity = 0;
	rollVelocity = 0;
	yawVelocity = 0;

	if (objLoaderCrashed.loadModel(&modelPath[0], crahsedPlane))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;
		crahsedPlane.calcCentrePoint();
		crahsedPlane.centreOnZero();
		crahsedPlane.calcVertNormalsUsingOctree();

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		crahsedPlane.initDrawElements();
		crahsedPlane.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	
	if (objLoaderCrashed.loadModel("models/plane/cockpit.obj", cockpit))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;


		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		cockpit.initDrawElements();
		cockpit.initVBO(shader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}


	testPointsCrahsed = {};

}



void plane::processKeys(float forward, float boost, float rollLeft, float rollRight, float pitchUp, float pitchDown, float yawLeft, float yawRight, float deltaTime,gameObj* ground, gameObj worldObj[], int noObjs, plane* otherPlane)
{
	if (!crashed) {

		for (glm::vec3 point : testPoints) {
			point = glm::vec3(modelMatrix * objectRotationMatrix * glm::vec4(point, 1));
			glm::mat4 inverse = glm::inverse(otherPlane->modelMatrix * otherPlane->objectRotationMatrix);

			point = glm::vec3(inverse * glm::vec4(point, 1));

			int a = 0;
			if (otherPlane->model.checkPointOctree(point, a)) {
				crashed = true;
				return;
			}
		}

		for (int i = 0; i < noObjs; i++) {
			for (glm::vec3 point : testPoints) {
				point = glm::vec3(modelMatrix * objectRotationMatrix * glm::vec4(point, 1));
				glm::mat4 inverse = glm::inverse(worldObj[i].modelMatrix * worldObj[i].objectRotationMatrix);

				point = glm::vec3(inverse * glm::vec4(point, 1));
				
				int a = 0;
				if (worldObj[i].model.checkPointOctree(point,a)) {
					crashed = true;
					return;
				}
			}
		}
		collidingPoints.clear();
		cout << endl;
		cout << "wheels: " << endl;
		for (glm::vec3 point : wheels) {
			point = glm::vec3(modelMatrix * objectRotationMatrix * glm::vec4(point, 1));
			glm::mat4 inverse = glm::inverse(ground->modelMatrix * ground->objectRotationMatrix);
			point = glm::vec3(inverse * glm::vec4(point, 1));

			if (ground->model.checkPointOctree(point).size() > 0) {
				collidingPoints.push_back(point);
				
			}
		}
		float moveXinc = 0.0f, moveYinc = 0.0f, moveZinc = 0.0f;
	
		std::vector<float> check;
		cout << endl;
		cout << "tip: " << endl;
		for (glm::vec3 point : testPoints) {
			point = glm::vec3(modelMatrix * objectRotationMatrix * glm::vec4(point, 1));
			glm::mat4 inverse = glm::inverse(ground->modelMatrix * ground->objectRotationMatrix);

			point = glm::vec3(inverse * glm::vec4(point, 1));

			check = ground->model.checkPointOctree(point);
		}

		if (check.size() > 0 && (velocity<maxVelocity/2)) {
			pos.y += 10;
		}
		else if (check.size() > 0 && (velocity > maxVelocity / 2)){
			crashed = true;
			return;
		}

		if (forward)
		{
			if (velocity == 0.0f) {
				velocity = 0.1f;
			}
			velocity += 1.2 * velocity * deltaTime;
			if (velocity > maxVelocity) {
				velocity = maxVelocity;
			}
			moveZinc = velocity;
		}
		else {
			velocity -= 0.0001 + (maxVelocity - velocity) * 0.5 * (double)deltaTime;
			if (velocity < 0.1f) {
				velocity = 0.0;
			}
			moveZinc = velocity;
		}

		/*else {
			velocity = 0;
			moveZinc = velocity;
			crashed = true;
		}*/
		//cout << velocity << endl;


		float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;

		if (rollLeft)
		{
			rollVelocity = rollVelocity - 2 * deltaTime;
			if (rollVelocity < -2) {
				rollVelocity = -2;
			}
		}
		if (rollRight)
		{
			rollVelocity = rollVelocity + 2 * deltaTime;
			if (rollVelocity > 2) {
				rollVelocity = 2;
			}
		}

		if (!rollLeft && !rollRight) {
			if (rollVelocity > 0) {
				rollVelocity = rollVelocity - 4 * deltaTime;
				if (rollVelocity < 0) {
					rollVelocity = 0;
				}
			}

			if (rollVelocity < 0) {
				rollVelocity = rollVelocity + 4 * deltaTime;
				if (rollVelocity > 0) {
					rollVelocity = 0;
				}
			}
		}
		spinZinc = rollVelocity * velocity / maxVelocity;



		if (pitchUp)
		{
			pitchVelocity = pitchVelocity + 2 * deltaTime;
			if (pitchVelocity > 1) {
				pitchVelocity = 1;
			}
		}
		if (pitchDown)
		{
			pitchVelocity = pitchVelocity - 2 * deltaTime;
			if (pitchVelocity < -1) {
				pitchVelocity = -1;
			}
		}

		if (!pitchUp && !pitchDown) {
			if (pitchVelocity > 0) {
				pitchVelocity = pitchVelocity - 4 * deltaTime;
				if (pitchVelocity < 0) {
					pitchVelocity = 0;
				}
			}

			if (pitchVelocity < 0) {
				pitchVelocity = pitchVelocity + 4 * deltaTime;
				if (pitchVelocity > 0) {
					pitchVelocity = 0;
				}
			}
		}
		spinXinc = pitchVelocity * velocity / maxVelocity;



		if (yawLeft)
		{
			yawVelocity = yawVelocity + 1 * deltaTime;
			if (yawVelocity > 0.25) {
				yawVelocity = 0.25;
			}
		}
		if (yawRight)
		{
			yawVelocity = yawVelocity - 1 * deltaTime;
			if (yawVelocity < -0.25) {
				yawVelocity = -0.25;
			}
		}

		if (!yawLeft && !yawRight) {
			if (yawVelocity > 0) {
				yawVelocity = yawVelocity - 4 * deltaTime;
				if (yawVelocity < 0) {
					yawVelocity = 0;
				}
			}

			if (yawVelocity < 0) {
				yawVelocity = yawVelocity + 4 * deltaTime;
				if (yawVelocity > 0) {
					yawVelocity = 0;
				}
			}
		}
		spinYinc = yawVelocity * velocity / maxVelocity;

		updatePos(spinXinc, spinYinc, spinZinc, moveXinc, moveYinc, moveZinc, deltaTime);
	}
	if (collidingPoints.size() == 0) {
		//updateMovement(forward, boost, rollLeft, rollRight, pitchUp, pitchDown, yawLeft, yawRight, deltaTime);
		if (gravVelocity == 0.0) {
			gravVelocity = 0.001;
		}
		gravVelocity += 0.75 * gravity;
		gravVelocity = gravVelocity * (1.2 - (velocity / maxVelocity));

		//gravVelocity *= deltaTime;
		if (gravVelocity > 100) {
			gravVelocity = 100;
		}
		//std::cout << gravVelocity << std::endl;

		pos.y += -gravVelocity * deltaTime;

		modelMatrix = glm::translate(glm::mat4(1.0f), pos);
	}
	else {
		gravVelocity = 0.001;
	}
}

bool plane::testCollision(glm::vec3 testPoint)
{
	return false;
}

gameObj* plane::testCollision(gameObj worldObj[], int noObjs, std::vector<glm::vec3> testPoints)
{
	return nullptr;
}

void plane::drawPlane(glm::mat4 viewingMatrix)
{
	glUseProgram(myShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	modelViewMatrix = viewingMatrix * modelMatrix * objectRotationMatrix;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
	glm::mat4 temp = modelMatrix * objectRotationMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, &LightPos[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, &Light_Specular[0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, &Material_Ambient[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, &Material_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, &Material_Specular[0]);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);

	if (crashed) {
		crahsedPlane.drawElementsUsingVBO(myShader);
	} else {
		model.drawElementsUsingVBO(myShader);
	}
	//model.drawOctreeLeaves(myShader);
	//cout << pos.x << endl;
}

void plane::drawPlaneCockpit(glm::mat4 viewingMatrix)
{
	glUseProgram(myShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	modelViewMatrix = viewingMatrix * modelMatrix * objectRotationMatrix;
	glm::mat4 temp = modelMatrix * objectRotationMatrix;
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));

	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, &LightPos[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, &Light_Ambient_And_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, &Light_Specular[0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, &Material_Ambient[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, &Material_Diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, &Material_Specular[0]);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);

	cockpit.drawElementsUsingVBO(myShader);
	//model.drawOctreeLeaves(myShader);
	//cout << pos.x << endl;
}

glm::vec3 plane::getLightPos()
{
	return glm::vec3(modelMatrix * objectRotationMatrix * glm::vec4(lightPos, 1.0));
}

glm::vec3 plane::getLightDir()
{
	return glm::vec3(objectRotationMatrix * glm::vec4(lightDir, 1.0));
}

