#include <iostream>
using namespace std;

//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "shaders\Shader.h"

glm::mat4 objectRotation;

Shader* myShader;  ///shader object 
Shader* myBasicShader;
Shader* groundShader;
Shader* coneShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
//#include "sphere/Sphere.h"
#include "gameObj.h"
#include "skyBox.h"
#include "plane.h"

#include <iostream> 
#include <algorithm>


#include <chrono>
double deltaTime = 0.0;
std::chrono::time_point<std::chrono::steady_clock> oldTime;

///END MODEL LOADING

glm::mat4 modelmatrix;
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.

////Material properties
//float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
//float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
//float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
//float Material_Shininess = 50;
//
////Light Properties
//float Light_Ambient_And_Diffuse[4] = {0.9f, 0.9f, 0.9f, 1.0f};
//float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
//float LightPos[4] = {0.2f, 1.0f, -1.0f, 0.0f};


//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
float Material_Specular[4] = { 0.04f,0.04f,0.04f,1.0f };
float Material_Shininess = 10;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 1.0f,0.5f,0.4f, 1.0f };
float Light_Specular[4] = { 1.0f,0.5f,0.4f,1.0f };
float LightPos[4] = { 1.0f, 0.3f, -1.0f, 0.0f };


int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=600, screenHeight=600;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool rollLeft = false;
bool rollRight = false;

bool w = false;
bool a = false;
bool s = false;
bool d = false;

bool boost = false;

float speed=0;

//Collision vars

glm::vec3 testPointCoords;

glm::vec3 boxPos = glm::vec3(0.0f, 0.0f, 0.0f);

//World Models
plane playerPlane;
plane aiPlane;

gameObj ground;

gameObj worldObj[8];
int noObjs = 8;

skyBox skybox;

//Cameras variables
int cameraState;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc, float Movexinc, float Moveyinc, float Movezinc);

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	//work out time since last frame
	auto start = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = start - oldTime;
	deltaTime = elapsed.count();
	oldTime = start;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(coneShader->handle());  // use the shader


	GLuint matLocation = glGetUniformLocation(coneShader->handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	objectRotation = playerPlane.objectRotationMatrix;
	glm::vec4 lookFromPos = glm::vec4(playerPlane.pos, 1);
	glm::vec4 lookAtPos = glm::vec4(playerPlane.pos, 1);

	//viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), playerPlane.pos, glm::vec3(0.0f, 1.0f, 0.0));
	if (cameraState == 1) {

		glm::vec3 res = glm::mat3(objectRotation) * glm::vec3(0, 15, -30.0);
		lookFromPos += glm::vec4(res, 0.0);
		res = glm::mat3(objectRotation) * glm::vec3(0, 0, 5);
		lookAtPos += glm::vec4(res, 0.0);
		viewingMatrix = glm::lookAt(glm::vec3(lookFromPos), glm::vec3(lookAtPos), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));
	}
	if (cameraState == 2) {

		glm::vec3 res = glm::mat3(objectRotation) * glm::vec3(0.0, 2, 5);
		lookFromPos += glm::vec4(res, 0.0);
		res = glm::mat3(objectRotation) * glm::vec3(0.0, 0.0, 20);
		lookAtPos += glm::vec4(res, 0.0);
		viewingMatrix = glm::lookAt(glm::vec3(lookFromPos), glm::vec3(lookAtPos), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));

	}
	if (cameraState == 3) {
		viewingMatrix = glm::lookAt(glm::vec3(0, 1200, 0), playerPlane.pos, glm::vec3(0.0f, 1.0f, 0.0));
	}

	glUniformMatrix4fv(glGetUniformLocation(coneShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	//glm::vec3 temp = glm::vec3(viewingMatrix * glm::vec4(playerPlane.getLightPos(), 1.0));
	glm::vec3 temp = glm::mat3(viewingMatrix) * playerPlane.getLightPos();
	glUniform3fv(glGetUniformLocation(coneShader->handle(), "lightPosition"), 1, &playerPlane.getLightPos()[0]);
	temp = glm::mat3(viewingMatrix) * playerPlane.getLightDir();
	glUniform3fv(glGetUniformLocation(coneShader->handle(), "lightDirection"), 1, &playerPlane.getLightDir()[0]);
	glUniform1f(glGetUniformLocation(coneShader->handle(), "lightCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(coneShader->handle(), "lightCutOffOuter"), glm::cos(glm::radians(17.5f)));
	glUniform3fv(glGetUniformLocation(coneShader->handle(), "planePos"), 1, &playerPlane.pos[0]);



	glUseProgram(myShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);


	//planeTemp.draw(viewingMatrix,myShader);
	if (cameraState == 1 || cameraState == 3) {
		playerPlane.drawPlane(viewingMatrix);
	}
	else {
		playerPlane.drawPlaneCockpit(viewingMatrix);
	}

	aiPlane.draw(viewingMatrix);


	worldObj[0].draw(viewingMatrix);

	worldObj[1].draw(viewingMatrix);

	worldObj[2].draw(viewingMatrix);

	worldObj[3].draw(viewingMatrix);
	
	worldObj[4].draw(viewingMatrix);

	worldObj[5].draw(viewingMatrix);

	worldObj[6].draw(viewingMatrix);

	worldObj[7].draw(viewingMatrix);
	
	//glUseProgram(groundShader->handle());

	//matLocation = glGetUniformLocation(groundShader->handle(), "ProjectionMatrix");
	//glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//glUniformMatrix4fv(glGetUniformLocation(groundShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);



	ground.draw(viewingMatrix);
	
	skybox.render(viewingMatrix, ProjectionMatrix);

	//cout << 1 / deltaTime << endl;
	
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(75.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 20000.0f);
}
void init()
{
	glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);

	myShader = new Shader();
    if(!myShader->load("defualtShader", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	coneShader = new Shader();
	//if (!myShader->load("defualtShader", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	if (!coneShader->load("coneShader", "glslfiles/conelighting.vert", "glslfiles/conelighting.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(coneShader->handle());  // use the shader

	glEnable(GL_TEXTURE_2D);

	cout << " loading model " << endl;

	playerPlane.init("models/plane/crashed.obj","models/plane/plane.obj", myShader);
	playerPlane.updatePos(glm::vec3(0.0, 2000, 5.0));
	playerPlane.Material_Specular = glm::vec4(0.8, 0.8, 0.8, 1.0);
	playerPlane.Material_Shininess = 40;
	playerPlane.velocity = playerPlane.maxVelocity;

	aiPlane.init("models/plane/plane.obj", "models/plane/plane.obj", coneShader);
	glm::vec3 axis =glm::vec3(1.0, 0.0, 0.0);
	auto tempRotMat = glm::rotate(aiPlane.objectRotationMatrix, glm::radians(5.0f), axis);
	aiPlane.objectRotationMatrix = tempRotMat;
	aiPlane.updatePos(glm::vec3(0.0, 2000, 0.0));
	aiPlane.velocity = aiPlane.maxVelocity;


	worldObj[0].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[0].updatePos(glm::vec3(500, 1200, -100));
	worldObj[0].scale(25, 25, 25);

	worldObj[1].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[1].updatePos(glm::vec3(600, 1200, 100));
	worldObj[1].scale(25, 25, 25);

	worldObj[2].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[2].updatePos(glm::vec3(100, 1200, 100));
	worldObj[2].scale(25, 25, 25);

	worldObj[3].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[3].updatePos(glm::vec3(200, 1200, -400));
	worldObj[3].scale(25, 25, 25);

	worldObj[4].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[4].updatePos(glm::vec3(-160, 1200, -600));
	worldObj[4].scale(25, 25, 25);

	worldObj[5].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[5].updatePos(glm::vec3(-300, 1200, -50));
	worldObj[5].scale(25, 25, 25);

	worldObj[6].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[6].updatePos(glm::vec3(250, 1200, -200));
	worldObj[6].scale(25, 25, 25);

	worldObj[7].init("models/rocks/rock1.obj", coneShader, true);
	worldObj[7].updatePos(glm::vec3(-60, 1200, 300));
	worldObj[7].scale(25, 25, 25);
	

	Shader *shader = new Shader();
	if (!shader->load("skyBoxShader", "glslfiles/skyBoxVertShader.vert", "glslfiles/skyBoxFragShader.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}

	skybox.init(shader);

	groundShader = new Shader();
	if (!groundShader->load("groundShader", "glslfiles/ground.vert", "glslfiles/ground.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}

	glUseProgram(coneShader->handle());
	//ground1.init("models/newWorld/groundDetailed.obj", groundShader, false);
	//ground1.scale(500, 500, 500);

	
	ground.init("models/newWorld/groundDetailed.obj", coneShader, false);
	ground.scale(1000, 1000, 1000);

	cameraState = 1;
	oldTime = std::chrono::steady_clock::now();
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	case GLUT_KEY_SHIFT_L:
		boost = true;
		break;
	}
}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	case GLUT_KEY_SHIFT_L:
		boost = false;
		break;
	case GLUT_KEY_F11:
		glutFullScreenToggle();
		break;
	}
}

void keyfunctionUp(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		w = false;
	}
	if (key == 'a' || key == 'A')
	{
		a = false;
	}
	if (key == 's' || key == 'S')
	{
		s = false;
	}
	if (key == 'd' || key == 'D')
	{
		d = false;
	}
	if (key == 'c' || key == 'C')
	{
		cameraState+=1;
		if (cameraState > 3) {
			cameraState = 1;
		}
	}
}

void keyfunction(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		//leaves the main loop and essentially exits the programme.
		glutLeaveMainLoop();
	}

	if (key == 'f' || key == 'F')
	{
		glutFullScreenToggle();
	}
	if (key == 'w' || key == 'W')
	{
		w = true;
	}
	if (key == 'a' || key == 'A')
	{
		a = true;
	}
	if (key == 's' || key == 'S')
	{
		s = true;
	}
	if (key == 'd' || key == 'D')
	{
		d = true;
	}
}

void idle()
{
	playerPlane.processKeys(w,boost,Left,Right,Up,Down,a,d,deltaTime,&ground,worldObj,noObjs,&aiPlane);
	aiPlane.processKeys(true, false, false, false, false, false, true, false, deltaTime, &ground, worldObj, noObjs,&playerPlane);
	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Flight Sim");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(keyfunction);
	glutKeyboardUpFunc(keyfunctionUp);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
