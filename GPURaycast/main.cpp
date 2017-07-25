#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>
#include <string>
#include <time.h>
#include "Defines.h"
#include "EulerCamera.h"
#include "Shader.h"
#include "Utilities.h"

#include "VolumeDataset.h"
#include "Texture3D.h"
#include "Framebuffer.h"
#include <cstdarg>

using namespace std;

const float width = 900, height = 900;
/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/
Framebuffer fb;
/*----------------------------------------------------------------------------
							CAMERA VARIABLES
----------------------------------------------------------------------------*/
EulerCamera camera;
int yawInput = 0, pitchInput = 0, rollInput = 0, forwardInput = 0, rightInput = 0;;
/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/
VolumeDataset volume;
Texture3D* volumeTexture3D, blankTexture3D;
clock_t oldTime;
int currentTimestep = 0;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

GLuint overlayVAO, overlayID, transfuncShaderID, computeShaderID, inverseComputeShaderID, Compute3DShaderID, tex_output;
int tex_w = width*3, tex_h = height*3;

void init()
{
	fb.init(width, height);
	Shader factory;
	computeShaderID = factory.CompileComputeShader(RAY_COMPUTE_SHADER);
	inverseComputeShaderID = factory.CompileComputeShader(INVERSE_COMPUTE_SHADER);
	Compute3DShaderID = factory.CompileComputeShader(COMPUTE_3D_SHADER);
	overlayVAO = createOverlayQuad();
	overlayID = factory.CompileShader(VERTEX_SHADER, FRAGMENT_SHADER);
	transfuncShaderID = factory.CompileShader(TRANS_VERTEX_SHADER, TRANS_FRAGMENT_SHADER);
	DebugWorkGroups();
	glEnable(GL_DEPTH_TEST);

	camera = EulerCamera(glm::vec3(0.0, 0.0, -10.0), width, height);
	volume.Init();
	volumeTexture3D = new Texture3D(volume);
	tex_output = volumeTexture3D->GenerateBlankTexture(volume);

}

void display()
{
	static int total = 0;
	static int number = 0;
	static int average = 0;
	clock_t initial = clock();
	glUseProgram(Compute3DShaderID);
	glBindImageTexture(0, tex_output, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8);
	int texp = glGetUniformLocation(Compute3DShaderID, "tex_output");
	glUniform1i(texp, 0);
	glDispatchCompute((GLuint)volume.xRes/4, (GLuint)volume.yRes/4, (GLuint)volume.zRes/4);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//Probably around here you're gonna need to start setting up your variables to go into the Raycasting.
	glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	Raycast(volumeTexture3D->transferFunction, tex_output, transfuncShaderID, camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	Raycast(volumeTexture3D->transferFunction, volumeTexture3D->currTexture3D, transfuncShaderID, camera);

	glUseProgram(overlayID);
	glBindVertexArray(overlayVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fb.tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glutSwapBuffers();
	total += clock() - initial;
	number++;
	if (average != total / number)
	{
		average = total / number;
		cout << average << endl;
	}
}

void updateScene() 
{	
	if (volume.timesteps > 1)
	{
		clock_t currentTime = clock();
		float time = (currentTime - oldTime) / (float)CLOCKS_PER_SEC;

		if (time > volume.timePerFrame)
		{
			if (currentTimestep < volume.timesteps - 2)
				currentTimestep++;
			else
				currentTimestep = 0;

			oldTime = currentTime;

			volumeTexture3D->UpdateTexture(currentTimestep, volume);
		}
	}
	camera.changeFront(pitchInput, yawInput, 0.0);
	camera.movForward(forwardInput);
	camera.movRight(rightInput);

	glutPostRedisplay();
}

#pragma region INPUT FUNCTIONS

void keypress(unsigned char key, int x, int y) {
	switch (key)
	{
	case ((char)27):
		exit(0);
		break;
	case('w'):
	case('W'):
		forwardInput = 1;
		break;
	case('s'):
	case('S'):
		forwardInput = -1;
		break;
	case('a'):
	case('A'):
		rightInput = -1;
		break;
	case('d'):
	case('D'):
		rightInput = 1;
		break;
	case('q'):
	case('Q'):
		break;
	case('e'):
	case('E'):
		break;
	}
}

void keypressUp(unsigned char key, int x, int y){
	switch (key)
	{
	case('w'):
	case('W'):
	case('s'):
	case('S'):
		forwardInput = 0;
		break;
	case('a'):
	case('A'):
	case('d'):
	case('D'):
		rightInput = 0;
		break;
	case('q'):
	case('Q'):
	case('e'):
	case('E'):
		break;
	case(' '):
		break;
	}
}

void specialKeypress(int key, int x, int y){
	switch (key)
	{
	case (GLUT_KEY_SHIFT_L):
	case (GLUT_KEY_SHIFT_R):
		break;
	case (GLUT_KEY_LEFT):
		yawInput = -1;
		break;
	case (GLUT_KEY_RIGHT):
		yawInput = 1;
		break;
	case (GLUT_KEY_UP):
		pitchInput = 1;
		break;
	case (GLUT_KEY_DOWN):
		pitchInput = -1;
		break;
	case(GLUT_KEY_F2):
		break;
	case(GLUT_KEY_F3):
		break;
	case(GLUT_KEY_F4):
		break;
	case(GLUT_KEY_F5):
		break;
	}
}

void specialKeypressUp(int key, int x, int y){
	switch (key)
	{
	case (GLUT_KEY_SHIFT_L):
	case (GLUT_KEY_SHIFT_R):
		break;
	case (GLUT_KEY_LEFT):
	case (GLUT_KEY_RIGHT):
		yawInput = 0;
		break;
	case (GLUT_KEY_UP):
	case (GLUT_KEY_DOWN):
		pitchInput = 0;
		break;
	case(GLUT_KEY_F1):
		break;
	case(GLUT_KEY_F2):
	case(GLUT_KEY_F3):
		break;
	case(GLUT_KEY_F4):
	case(GLUT_KEY_F5):
		cout << camera.getFront().x << " " << camera.getFront().y << " " << camera.getFront().z << " " << endl;
		cout << camera.getPosition().x << " " << camera.getPosition().y << " " << camera.getPosition().z << " " << endl;
		cout << camera.yaw << endl;
		break;
	}
}

void (mouse)(int x, int y)
{

}

#pragma endregion INPUT FUNCTIONS

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("GameApp");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


	// Tell glut where the display function is
	glutWarpPointer(width / 2, height / 2);
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);

	// Input Function Initialisers //
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouse);
	glutSpecialFunc(specialKeypress);
	glutSpecialUpFunc(specialKeypressUp);
	glutKeyboardUpFunc(keypressUp);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	init();
	glutMainLoop();
	return 0;
}