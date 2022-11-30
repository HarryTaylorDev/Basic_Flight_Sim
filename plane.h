#pragma once
#include "gameObj.h"
//#include "Obj\OBJLoader.h"

class plane : public gameObj
{
private:

public:
	float gravity;
	float gravVelocity;
	float velocity;
	float maxVelocity;
	float pitchVelocity;
	float rollVelocity;
	float yawVelocity;
	float speed;
	bool crashed;
	bool landed;
	ThreeDModel crahsedPlane;
	ThreeDModel defualtPlane;
	ThreeDModel cockpit;

	OBJLoader objLoaderCrashed;
	std::vector<glm::vec3> wheels = {
		glm::vec3(0.0,-3.767,4.326),
		glm::vec3(-2.616, -3.817, -3.913),
		glm::vec3(2.616, -3.817, -3.913)
	};

	std::vector<glm::vec3> testPoints = {
		glm::vec3(0.0,0.0,15.577),
		glm::vec3(0.0,6.75,-14.25),
		glm::vec3(-10.36,0.0,-6.32),
		glm::vec3(10.36,0.0,-6.32),
	};

	glm::vec3 lightPos = glm::vec3(0.0, -15.577, 0.0);
	glm::vec3 lightDir = glm::vec3(0.0, 0.0, 1.0);

	
	std::vector<glm::vec3> testPointsCrahsed;//add the points for the new plane model

	std::vector<glm::vec3> collidingPoints;
	plane();
	void init(string modelPath, string modelPathCrashed, Shader *shader);
	void updateMovement(float forward, float boost, float rollLeft, float rollRight, float pitchUp, float pitchDown, float yawLeft, float yawRight, float deltaTime);
	void processKeys(float forward, float boost, float rollLeft, float rollRight, float pitchUp, float pitchDown, float yawLeft, float yawRight, float deltaTime, gameObj* ground, gameObj worldObj[], int noObjs, plane* otherPlane);
	
	bool testCollision(glm::vec3 testPoint);
	gameObj* testCollision(gameObj worldObj[], int noObjs, std::vector<glm::vec3> testPoints);

	void drawPlane(glm::mat4 viewingMatrix);
	void drawPlaneCockpit(glm::mat4 viewingMatrix);

	glm::vec3 getLightPos();
	glm::vec3 getLightDir();

};

