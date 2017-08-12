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
#include <cstdarg>

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
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

GLuint visibilityVAO, volumeVAO, overlayID, transfuncShaderID, computeShaderID, inverseComputeShaderID, LaplaceShaderID, GaussianShaderID;
GLuint visibilityShaderID;
int tex_w = width*3, tex_h = height*3;

void init()
{
	fbVisibility.init(width, height);
	fbVolume.init(width, height);
	Shader factory;
	computeShaderID = factory.CompileComputeShader(RAY_COMPUTE_SHADER);
	inverseComputeShaderID = factory.CompileComputeShader(INVERSE_COMPUTE_SHADER);
	LaplaceShaderID = factory.CompileComputeShader(LAPLACIAN_COMPUTE_SHADER);
	GaussianShaderID = factory.CompileComputeShader(GAUSSIAN_COMPUTE_SHADER);
	visibilityShaderID = factory.CompileComputeShader(VISIBILITY_COMPUTE_SHADER);
	volumeVAO = createOverlayQuad(5);
	visibilityVAO = createOverlayQuad(4);
	overlayID = factory.CompileShader(VERTEX_SHADER, FRAGMENT_SHADER);
	transfuncShaderID = factory.CompileShader(TRANS_VERTEX_SHADER, TRANS_FRAGMENT_SHADER);
	DebugWorkGroups();
	glEnable(GL_DEPTH_TEST);

	dataVolumeCamera = EulerCamera(glm::vec3(0.0, 0.0, -5.0), width/2, height);
	overlayCamera = EulerCamera(glm::vec3(0.0, 0.0, -5.0), width/2, height);
	volume.Init();
	volumeContainer3D = new VolumeTexture(volume);

}

void display()
{
	static vector<int> numberBacklog;
	int total = 0;
	static int average = 0;
	clock_t initial = clock();

	if (laplacianFinished != true)
	{
		LaunchComputeShader(GaussianShaderID, volumeContainer3D->dataTexture, volumeContainer3D->smoothedTexture, volume);
		LaunchComputeShader(LaplaceShaderID, volumeContainer3D->smoothedTexture, volumeContainer3D->laplacianTexture, volume);
		laplacianFinished = true;
	}

	LaunchVisibilityComputeShader(volumeContainer3D, visibilityShaderID, dataVolumeCamera, volume);

	//Probably around here you're gonna need to start setting up your variables to go into the Raycasting.
	glBindFramebuffer(GL_FRAMEBUFFER, fbVisibility.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	if(renderLaplace)
		Raycast(volumeContainer3D->visibilityTF, volumeContainer3D->laplacianTexture, transfuncShaderID, overlayCamera);
	else	
		Raycast(volumeContainer3D->visibilityTF, volumeContainer3D->visibilityTexture, transfuncShaderID, overlayCamera);


	glBindFramebuffer(GL_FRAMEBUFFER, fbVolume.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	Raycast(volumeContainer3D->dataTF, volumeContainer3D->dataTexture, transfuncShaderID, dataVolumeCamera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

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

	numberBacklog.push_back(clock() - initial);
	if (numberBacklog.size() > 20)
		numberBacklog.erase(numberBacklog.begin());
	for (int i = 0; i < numberBacklog.size(); i++)
		total += numberBacklog[i];
	if (average != total / numberBacklog.size())
	{
		average = total / numberBacklog.size();
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

			volumeContainer3D->UpdateTexture(currentTimestep, volume);
		}
	}
	dataVolumeCamera.orbitAround(glm::vec3(0.0, 0.0, 0.0), rotateDataZ, rotateDataY);

	overlayCamera.orbitAround(glm::vec3(0.0, 0.0, 0.0), rotateVisualZ, rotateVisualY);


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
	}
}

void specialKeypress(int key, int x, int y){
	switch (key)
	{
	case (GLUT_KEY_SHIFT_L):
	case (GLUT_KEY_SHIFT_R):
		break;
	case (GLUT_KEY_LEFT):
		dataVolumeCamera.yawInput = -1;
		break;
	case (GLUT_KEY_RIGHT):
		dataVolumeCamera.yawInput = 1;
		break;
	case (GLUT_KEY_UP):
		dataVolumeCamera.pitchInput = 1;
		break;
	case (GLUT_KEY_DOWN):
		dataVolumeCamera.pitchInput = -1;
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
		dataVolumeCamera.yawInput = 0;
		break;
	case (GLUT_KEY_UP):
	case (GLUT_KEY_DOWN):
		dataVolumeCamera.pitchInput = 0;
		break;
	case(GLUT_KEY_F1):
		break;
	case(GLUT_KEY_F2):
	case(GLUT_KEY_F3):
		break;
	case(GLUT_KEY_F4):
	case(GLUT_KEY_F5):
		cout << dataVolumeCamera.getFront().x << " " << dataVolumeCamera.getFront().y << " " << dataVolumeCamera.getFront().z << " " << endl;
		cout << dataVolumeCamera.getPosition().x << " " << dataVolumeCamera.getPosition().y << " " << dataVolumeCamera.getPosition().z << " " << endl;
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