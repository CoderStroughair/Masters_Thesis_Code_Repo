#ifndef VOLUMETEXTURE_H
#define VOLUMETEXTURE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "VolumeDataset.h"
#include "TransferFunction.h"

class VolumeTexture
{
public:
	TransferFunction visibilityTF;
	TransferFunction laplacianTF;

	GLuint dataTexture;
	GLuint laplacianTexture;
	GLuint smoothedTexture;
	GLuint visibilityTexture;
	int textureSize;

	GLuint GenerateTexture(VolumeDataset &volume);
	GLuint GenerateBlankTexture(VolumeDataset &volume);

	VolumeTexture() {};
	VolumeTexture(VolumeDataset &volume);
	void UpdateTexture(int currentTimestep, VolumeDataset &volume);
};
#endif