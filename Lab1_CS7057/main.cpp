#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>
#include <string>
#include "Defines.h"
#include "EulerCamera.h"
#include "Shader.h"
#include "Utilities.h"

#include "VolumeDataset.h"

using namespace std;

const float width = 900, height = 900;
/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
							CAMERA VARIABLES
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/
VolumeDataset volume;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

GLuint quad_vao, graphicPipelineID, computeShaderID, inverseComputeShaderID, tex_output;
int tex_w = width, tex_h = height;

void init()
{
	Shader factory;
	computeShaderID = factory.CompileComputeShader(RAY_COMPUTE_SHADER);
	inverseComputeShaderID = factory.CompileComputeShader(INVERSE_COMPUTE_SHADER);
	quad_vao = quadVAO();
	graphicPipelineID = factory.CompileShader(VERTEX_SHADER, FRAGMENT_SHADER);
	tex_output = BlankTexture(tex_w, tex_h);
	DebugWorkGroups();
	glEnable(GL_DEPTH_TEST);

	//initialise camera
	volume.Init();
	printf(KNEE_HEADER);
}

void display()
{

	glUseProgram(computeShaderID);
	glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);

	// prevent sampling befor all writes to image are done
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(inverseComputeShaderID);
	glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);

	// prevent sampling befor all writes to image are done
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(graphicPipelineID);
	glBindVertexArray(quad_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glutSwapBuffers();
}

void updateScene() 
{	
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
		break;
	case('s'):
	case('S'):
		break;
	case('a'):
	case('A'):
		break;
	case('d'):
	case('D'):
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
		break;
	case('a'):
	case('A'):
	case('d'):
	case('D'):
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
		break;
	case (GLUT_KEY_RIGHT):
		break;
	case (GLUT_KEY_UP):
		break;
	case (GLUT_KEY_DOWN):
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
		break;
	case (GLUT_KEY_UP):
	case (GLUT_KEY_DOWN):
		break;
	case(GLUT_KEY_F1):
		break;
	case(GLUT_KEY_F2):
	case(GLUT_KEY_F3):
		break;
	case(GLUT_KEY_F4):
	case(GLUT_KEY_F5):
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