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
#include "VolumeTexture.h"
#include "Framebuffer.h"

using namespace std;

const float width = 900, height = 900;
/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/
Framebuffer fbVisibility, fbVolume;
/*----------------------------------------------------------------------------
							CAMERA VARIABLES
----------------------------------------------------------------------------*/
EulerCamera dataVolumeCamera, overlayCamera;
int rotateVisualZ = 0, rotateVisualY = 0;
int rotateDataZ = 0, rotateDataY = 0;
/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/
VolumeDataset volume;
VolumeTexture* volumeContainer3D;
clock_t oldTime;
int currentTimestep = 0;
bool laplacianFinished = false;

bool renderLaplace = false;
int changeLowerLimit = 0;
float visibilityLowerLimit = 0.0f;
float average = 0;
float total = 0;
int numNumbers = 0;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

GLuint visibilityVAO, volumeVAO, overlayID, transfuncShaderID, computeShaderID, inverseComputeShaderID, LaplaceShaderID, MaximaShaderID;
GLuint visibilityShaderID;
int tex_w = width*3, tex_h = height*3;

void init()
{
	fbVisibility.init(width, height);
	fbVolume.init(width, height);
	Shader shaderFactory;
	computeShaderID = shaderFactory.CompileComputeShader(RAY_COMPUTE_SHADER);
	inverseComputeShaderID = shaderFactory.CompileComputeShader(INVERSE_COMPUTE_SHADER);
	LaplaceShaderID = shaderFactory.CompileComputeShader(LAPLACIAN_COMPUTE_SHADER);
	MaximaShaderID = shaderFactory.CompileComputeShader(LOCAL_MAXIMA_COMPUTE_SHADER);
	visibilityShaderID = shaderFactory.CompileComputeShader(VISIBILITY_COMPUTE_SHADER);
	volumeVAO = createOverlayQuad(4);
	visibilityVAO = createOverlayQuad(5);
	overlayID = shaderFactory.CompileShader(VERTEX_SHADER, FRAGMENT_SHADER);
	transfuncShaderID = shaderFactory.CompileShader(TRANS_VERTEX_SHADER, TRANS_FRAGMENT_SHADER);
	DebugWorkGroups();

	glEnable(GL_DEPTH_TEST);

	dataVolumeCamera = EulerCamera(glm::vec3(0.0, 0.0, -5.0), width/2, height);
	overlayCamera = EulerCamera(glm::vec3(0.0, 0.0, -5.0), width/2, height);
	volume.Init();
	volumeContainer3D = new VolumeTexture(volume);

}

void display()
{
	static vector<float> numberBacklog;

	if (laplacianFinished != true)
	{
		LaunchComputeShader(LaplaceShaderID, volumeContainer3D->dataTexture, volumeContainer3D->laplacianTexture, volume, visibilityLowerLimit, volumeContainer3D->dataTF.tfTexture);
		laplacianFinished = true;
	}

	clock_t initial = clock();

	LaunchVisibilityComputeShader(volumeContainer3D, visibilityShaderID, dataVolumeCamera, volume, visibilityLowerLimit);

	clock_t timeTaken = clock() - initial;
	//Probably around here you're gonna need to start setting up your variables to go into the Raycasting.

	glBindFramebuffer(GL_FRAMEBUFFER, fbVolume.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	Raycast(volumeContainer3D->dataTF, volumeContainer3D->dataTexture, transfuncShaderID, dataVolumeCamera);

	glBindFramebuffer(GL_FRAMEBUFFER, fbVisibility.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	if (renderLaplace)
	{
		Raycast(volumeContainer3D->laplacianTF, volumeContainer3D->laplacianTexture, transfuncShaderID, overlayCamera);
	}
	else
	{
		Raycast(volumeContainer3D->visibilityTF, volumeContainer3D->visibilityTexture, transfuncShaderID, overlayCamera);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glUseProgram(overlayID);
	glBindVertexArray(visibilityVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbVisibility.tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(volumeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbVolume.tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glutSwapBuffers();

	total += timeTaken;
	numNumbers++;
	average = total / numNumbers;
	if (numNumbers == 360)
	{
		cout << "Average of 360 frames: " << average/CLOCKS_PER_SEC * 1000 <<"ms"<< endl;
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

			volumeContainer3D->UpdateTexture(currentTimestep, volume);
		}
	}
	dataVolumeCamera.orbitAround(glm::vec3(0.0, 0.0, 0.0), 1, 1);

	overlayCamera.orbitAround(glm::vec3(0.0, 0.0, 0.0), rotateVisualZ, rotateVisualY);

	visibilityLowerLimit += changeLowerLimit / 10.0;

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
		rotateDataZ = -1;
		break;
	case('s'):
	case('S'):
		rotateDataZ = 1;
		break;
	case('a'):
	case('A'):
		rotateDataY = -1;
		break;
	case('d'):
	case('D'):
		rotateDataY = 1;
		break;
	case('q'):
	case('Q'):
		break;
	case('e'):
	case('E'):
		break;
	case('i'):
	case('I'):
		rotateVisualZ = -1;
		break;
	case('k'):
	case('K'):
		rotateVisualZ = 1;
		break;
	case('j'):
	case('J'):
		rotateVisualY = -1;
		break;
	case('l'):
	case('L'):
		rotateVisualY = 1;
		break;
	}
}

void keypressUp(unsigned char key, int x, int y){
	switch (key)
	{
	case('w'):
	case('s'):
	case('W'):
	case('S'):
		rotateDataZ = 0;
		break;
	case('a'):
	case('d'):
	case('A'):
	case('D'):
		rotateDataY = 0;
		break;
	case('q'):
	case('e'):
	case('Q'):
	case('E'):
		break;
	case('i'):
	case('I'):
	case('k'):
	case('K'):
		rotateVisualZ = 0;
		break;
	case('j'):
	case('J'):
	case('l'):
	case('L'):
		rotateVisualY = 0;
		break;
	case(' '):
		renderLaplace = !renderLaplace;
		break;
	case('p'):
	case('P'):
		cout << visibilityLowerLimit << endl;
		break;
	}
}

void specialKeypress(int key, int x, int y){
	switch (key)
	{
	case (GLUT_KEY_LEFT):
		break;
	case (GLUT_KEY_RIGHT):
		break;
	case (GLUT_KEY_UP):
		break;
	case (GLUT_KEY_DOWN):
		break;
	case(GLUT_KEY_F1):
		changeLowerLimit = 1;
		if(renderLaplace)
		laplacianFinished = false;
		break;
	case(GLUT_KEY_F2):
		changeLowerLimit = -1;
		if(renderLaplace)
		laplacianFinished = false;
		break;
	}
}

void specialKeypressUp(int key, int x, int y){
	switch (key)
	{
	case (GLUT_KEY_LEFT):
	case (GLUT_KEY_RIGHT):
		break;
	case (GLUT_KEY_UP):
	case (GLUT_KEY_DOWN):
		break;
	case(GLUT_KEY_F1):
	case(GLUT_KEY_F2):
		changeLowerLimit = 0;
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