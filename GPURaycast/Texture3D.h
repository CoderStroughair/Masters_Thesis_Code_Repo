#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "VolumeDataset.h"
#include "TransferFunction.h"

class Texture3D
{
public:
	TransferFunction volumeTransferFunction;
	TransferFunction computedTransferFunction;
	GLuint volumeTexture3D;
	GLuint computedTexture3D;
	GLuint smoothedTexture3D;
	int textureSize;

	GLuint GenerateTexture(VolumeDataset &volume);
	GLuint GenerateBlankTexture(VolumeDataset &volume);

	Texture3D() {};
	Texture3D(VolumeDataset &volume);
	void UpdateTexture(int currentTimestep, VolumeDataset &volume);
};
#endif