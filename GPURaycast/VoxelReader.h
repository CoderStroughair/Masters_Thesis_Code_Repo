#ifndef VOXEL_READER_H
#define VOXEL_READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include <GL/freeglut.h>

struct VolumeProperties
{
	GLubyte* bufferAddress;
	int timesteps;
	float timePerFrame;
	int numDims;
	int xRes, yRes, zRes;
	std::string rawFilePath;
	int bytesPerElement;
	bool littleEndian;
	std::string elementType;

	VolumeProperties();
};

class VoxelReader
{
public:
	std::vector<std::string> files;

	void LoadVolume(std::string folderPath, std::string headerFile, VolumeProperties &properties);
	void CopyFileToBuffer(std::string fileName, int &numBytesInBufferFilled, VolumeProperties &properties);

private:
	void ReadMHD(std::string folderPath, std::string headerFile, VolumeProperties &properties);
	void ReadRaw(VolumeProperties &properties);
	
};


#endif