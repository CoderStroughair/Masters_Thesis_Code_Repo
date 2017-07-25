#ifndef EULERCAMERA_H
#define EULERCAMERA_H

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "GLM.h"
#include <GL/freeglut.h>


/**
Requirements :
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Init() Function :
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Input Functions :
pitCam, yawCam, rolCam - These become 1 to move in the positive direction, and -1 to move in the negative direction.The reset to 0 during the function call.
I recommend that they are changed via the mouse input option, in some shape or form.Set rolCam to 0 and never touch it, I didn't finish it yet.

frontCam, sideCam - These two become either 1 to go in the positive axis direction, 0 to not move, and -1 to go in the negative axis direction.
I recommend that they are changed via the glutKeyboardFunc and glutKeyboardUpFunc, or the GLFW equivalent.
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your UpdateScene() Function:
Call to cam.movForward(frontCam), cam.movRight(sideCam), and cam.changeFront(pitCam, yawCam, rolCam); .That's it.
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your display() Function:
view = look_at(cam.getPosition(), cam.getPosition() + cam.getFront(), cam.getUp());
**/

class EulerCamera {

public:
	EulerCamera() {};
	EulerCamera(glm::vec3 pos, int screenWidth, int screenHeight);
	void setSensitivity(GLfloat value);
	void changeFront(GLfloat pi, GLfloat ya, GLfloat ro);
	void movForward(GLfloat value);
	void movRight(GLfloat value);
	void move(GLfloat value);
	void setPosition(glm::vec3 value);
	glm::vec3 getPosition();
	glm::vec3 getFront();
	void setFront(glm::vec3 value, GLfloat y, GLfloat p);
	glm::vec3 getUp();	
	glm::mat4 getView();
	glm::mat4 getProj();
	void jump(bool& jumping);
	
	GLuint cam_buffer;
	glm::mat4 rot = glm::mat4();
	GLfloat yaw, pitch, roll;

private:
	glm::vec3 position, front, up, right, worldup;
	glm::mat4 proj;
	GLfloat sensitivity, degrees;

	void changeFront();
};

#endif