#include "VolumeDataset.h"
#include "Defines.h"

// Initializes volume by calling voxel reader and copying in values from header
void VolumeDataset::Init()
{
	VolumeProperties properties;
	voxelReader.LoadVolume(ENGINE_PATH, ENGINE_HEADER, properties);

	memblock3D = properties.bufferAddress;
	timesteps = properties.timesteps;
	timePerFrame = properties.timePerFrame;
	numDims = properties.numDims;
	xRes = properties.xRes;
	yRes = properties.yRes;
	zRes = properties.zRes;
	bytesPerElement = properties.bytesPerElement;
	littleEndian = properties.littleEndian;
	elementType = properties.elementType;

	std::cout << "Volume Dimensions:\n" << "X Res: " << xRes << "\n" << "Y Res: " << yRes << "\n" << "Z Res: " << zRes << "\n";

}

// Only needed if you want to analyse the volume on the CPU and the endianness is in the wrong order
void VolumeDataset::ReverseEndianness()
{
	std::vector<GLubyte> bytes;
	bytes.resize(xRes * yRes * zRes * bytesPerElement);

	for (int i=0; i<bytes.size(); i+=bytesPerElement)
	{
		for (int j=0; j<bytesPerElement; j++)
		{
			bytes[i+j] = memblock3D[i+(bytesPerElement-(1+j))];
		}
	}

	memcpy(memblock3D, &bytes[0], xRes * yRes * zRes * bytesPerElement);
}


