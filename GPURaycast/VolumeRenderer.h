#ifndef VOLUME_RENDERER_H
#define VOLUME_RENDERER_H

#include "Camera.h"
#include "VolumeDataset.h"
#include "ShaderManager.h"
#include "OpenGLRenderer.h"
#include <time.h>

class VolumeRenderer
{
public:
	Camera camera;
	GLuint shaderProgramID;
	VolumeDataset volume;
	OpenGLRenderer *renderer;

	clock_t oldTime;
	int currentTimestep;

	void Init(int screenWidth, int screenHeight);
	void Update();

};


#endif