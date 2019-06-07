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
#include <map>

using namespace std;

const int width = 900, height = 900;
/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/
Framebuffer fbVisibility, fbVolume;
/*----------------------------------------------------------------------------
							CAMERA VARIABLES
----------------------------------------------------------------------------*/
EulerCamera dataVolumeCamera, overlayCamera;
float rotateVisualZ = 0, rotateVisualY = 0;
float rotateDataZ = 0, rotateDataY = 0;
/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/
std::vector<TransferFunction> transferFunctions;
int tfIdx = 0;

const char* TFStrings[] = 
{
	TRANS_CTKNEE, TRANS_VISMALE, TRANS_BLUESMOKE, TRANS_NUCLEON, TRANS_MRI_KNEE,
	TRANS_MRI_KNEE_TRANS, TRANS_SUPERNOVA, TRANS_TEST, TRANS_TEST2, TRANS_TOOTH, 
	TRANS_TOOTH2, TRANS_TOOTH2LOW, TRANS_TOOTH2VLOW, TRANS_TOOTH2VVLOW, TRANS_ENGINE
};

void InitialiseTransferFunctions()
{
	for( int i = 0; i < (sizeof( TFStrings ) / sizeof( *TFStrings )); i++ )
	{
		TransferFunction TF;
		TF.Init(TFStrings[i]);
		transferFunctions.push_back( TF );
	}
}
/*----------------------------------------------------------------------------
							VOLUME VARIABLES
----------------------------------------------------------------------------*/

struct VolumeDataPaths
{
	const char* path;
	const char* fileName;
	VolumeDataPaths(const char* _path, const char* _fileName):
		path(_path),
		fileName(_fileName){}
};

VolumeDataPaths dataPaths[] = 
{
	VolumeDataPaths( BONSAI_PATH , BONSAI_HEADER ), 
	VolumeDataPaths( KNEE_PATH , KNEE_HEADER ),
	VolumeDataPaths( NUCLEON_PATH , NUCLEON_HEADER ),
	VolumeDataPaths( TOOTH_PATH , TOOTH_HEADER ),
	VolumeDataPaths( ENGINE_PATH , ENGINE_HEADER )
};
int volumeIdx = 0;
//std::vector<VolumeDataset> volumes;
VolumeDataset volume;
VolumeTexture* volumeContainer3D;
void InitialiseVolumeDataset()
{
	volume.Init( dataPaths[volumeIdx].path, dataPaths[volumeIdx].fileName );
	if( volumeContainer3D )
	{
		delete volumeContainer3D;
	}
	volumeContainer3D = new VolumeTexture( volume );
}
/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/
clock_t oldTime;
int currentTimestep = 0;
bool laplacianFinished = false;

bool renderLaplace = false;
int changeLowerLimit = 0;
float visibilityLowerLimit = 0.0f;
float average = 0;
float total = 0;
int numNumbers = 0;

float timeTillAutoRotate = 0;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

GLuint visibilityVAO, volumeVAO, overlayID, transfuncShaderID, computeShaderID, inverseComputeShaderID, LaplaceShaderID, MaximaShaderID, TextShaderID;
GLuint visibilityShaderID;
int tex_w = width*3, tex_h = height*3;

void init()
{
	InitialiseTransferFunctions();
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
	InitialiseVolumeDataset();
}

void display()
{
	static vector<float> numberBacklog;

	if (laplacianFinished != true)
	{
		LaunchComputeShader(LaplaceShaderID, volumeContainer3D->dataTexture, volumeContainer3D->laplacianTexture, volume, visibilityLowerLimit, transferFunctions[tfIdx].tfTexture);
		laplacianFinished = true;
	}

	clock_t initial = clock();

	LaunchVisibilityComputeShader(volumeContainer3D, transferFunctions[tfIdx], visibilityShaderID, dataVolumeCamera, volume, visibilityLowerLimit);

	clock_t timeTaken = clock() - initial;
	//Probably around here you're gonna need to start setting up your variables to go into the Raycasting.

	glBindFramebuffer(GL_FRAMEBUFFER, fbVolume.framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	Raycast( transferFunctions[tfIdx], volumeContainer3D->dataTexture, transfuncShaderID, dataVolumeCamera);

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
	clock_t currentTime = clock();
	float deltaTime = (currentTime - oldTime) / (float)CLOCKS_PER_SEC;
	if (volume.timesteps > 1)
	{
		if (deltaTime > volume.timePerFrame)
		{
			if (currentTimestep < volume.timesteps - 2)
				currentTimestep++;
			else
				currentTimestep = 0;

			volumeContainer3D->UpdateTexture(currentTimestep, volume);
		}
	}

	if( rotateDataY != 0 || rotateDataZ != 0 )
	{
		timeTillAutoRotate = 5;
	}
	if( timeTillAutoRotate > 0 )
	{
		timeTillAutoRotate -= deltaTime;
		dataVolumeCamera.orbitAround( glm::vec3( 0.0, 0.0, 0.0 ), rotateDataZ, rotateDataY );
	}
	else
	{
		dataVolumeCamera.orbitAround( glm::vec3( 0.0, 0.0, 0.0 ), 1, 1 );
	}

	overlayCamera.orbitAround(glm::vec3(0.0, 0.0, 0.0), rotateVisualZ, rotateVisualY);

	visibilityLowerLimit += (float)changeLowerLimit / 10.0f;

	oldTime = currentTime;

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
		rotateDataZ = -1.0f;
		break;
	case('s'):
	case('S'):
		rotateDataZ = 1.0f;
		break;
	case('a'):
	case('A'):
		rotateDataY = -1.0f;
		break;
	case('d'):
	case('D'):
		rotateDataY = 1.0f;
		break;
	case('q'):
	case('Q'):
		tfIdx++;
		if( tfIdx >= (sizeof( TFStrings ) / sizeof( *TFStrings )) )
		{
			tfIdx = 0;
		}
		break;
	case('e'):
	case('E'):
		tfIdx--;
		if( tfIdx < 0 )
		{
			tfIdx = (sizeof( TFStrings ) / sizeof( *TFStrings )) - 1;;
		}
		break;
	case('z'):
	case('Z'):
		volumeIdx++;
		if( volumeIdx >= (sizeof( dataPaths ) / sizeof( *dataPaths )) )
		{
			volumeIdx = 0;
		}
		InitialiseVolumeDataset();
		break;
	case('c'):
	case('C'):
		volumeIdx--;
		if( volumeIdx < 0 )
		{
			volumeIdx = (sizeof( dataPaths ) / sizeof( *dataPaths )) - 1;;
		}
		InitialiseVolumeDataset();
		break;
	case('i'):
	case('I'):
		rotateVisualZ = -1.0f;
		break;
	case('k'):
	case('K'):
		rotateVisualZ = 1.0f;
		break;
	case('j'):
	case('J'):
		rotateVisualY = -1.0f;
		break;
	case('l'):
	case('L'):
		rotateVisualY = 1.0f;
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
		rotateDataZ = 0.0f;
		break;
	case('a'):
	case('d'):
	case('A'):
	case('D'):
		rotateDataY = 0.0f;
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
		rotateVisualZ = 0.0f;
		break;
	case('j'):
	case('J'):
	case('l'):
	case('L'):
		rotateVisualY = 0.0f;
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
	glutCreateWindow( "Master's Project - Quantifying Visibility in Volume Rendering with Compute Shaders");
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