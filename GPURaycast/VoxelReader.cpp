#include "VoxelReader.h"

// At the moment file is specified within function but in future will use parameter. 
void VoxelReader::LoadVolume(std::string folderPath, std::string headerFile, VolumeProperties &properties)
{
	ReadMHD(folderPath, headerFile, properties);
	ReadRaw(properties);
}


// Reads a header of .mhd format and copies values to 'VolumeProperties'
void VoxelReader::ReadMHD(std::string folderPath, std::string headerFile, VolumeProperties &properties)
{
	std::ifstream myFile(headerFile);

	if (!myFile.is_open())
	{
		std::cout << "Failed to open File" << std::endl;
		return;
	}

	std::vector<std::string> fileLines;
	std::string line, temp, temp2;
	std::istringstream iss;

	while (getline(myFile, line))
		fileLines.push_back(line);

	for (int i=0; i<fileLines.size(); i++)
	{
		iss.str(fileLines[i]);
		iss >> temp;
		iss >> temp2;

		if (temp == "Timesteps")
		{
			iss >> properties.timesteps;
		}
		else if (temp == "NDims")
		{
			iss >> properties.numDims;
		}
		else if (temp == "DimSize")
		{
			iss >> properties.xRes;
			iss >> properties.yRes;
			iss >> properties.zRes;
		}
		else if (temp == "ElementType")
		{
			iss >> properties.elementType;

			if (properties.elementType == "MET_UCHAR")
				properties.bytesPerElement = 1;
			else if (properties.elementType == "SHORT")
				properties.bytesPerElement = 2;
			else if (properties.elementType == "FLOAT")
				properties.bytesPerElement = 4;
		}
		else if (temp == "ElementDataFile")
		{
			iss >> temp;
			properties.rawFilePath = folderPath;
			properties.rawFilePath.append(temp);
		}
		else if (temp == "ElementByteOrderMSB")
		{
			iss >> temp;

			if (temp == "TRUE" || temp == "True")
				properties.littleEndian = true;
			else
				properties.littleEndian = false;
		}
		iss.clear();
	}
}


// Basic sort function used to sort files numerically rather than per character
struct MyFileSort : public std::binary_function<std::string, std::string, bool>
{
	bool operator() (std::string &a, std::string &b) const
	{
		return (a.length() < b.length() || (a.length() == b.length() && (a < b)));
	}
};


// Reads in the raw binary data using properties copied in from header
void VoxelReader::ReadRaw(VolumeProperties &properties)
{
	// Allocate CPU memory block
	int bufferSize = properties.xRes * properties.yRes * properties.zRes * properties.bytesPerElement * properties.timesteps;
	properties.bufferAddress = new GLubyte [bufferSize];

	int numBytesInBufferFilled = 0;
	
	std::string directory = properties.rawFilePath;
	directory.append("/*");

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(directory.c_str(), &findFileData);

	// If multiple files in specified path then sort the contents and read them in sequentially
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindNextFileA(hFind, &findFileData);

		while (FindNextFileA(hFind, &findFileData) != 0)
			files.push_back(findFileData.cFileName);
		
		sort(files.begin(), files.end(), MyFileSort());

		for (int i=0; i<files.size(); i++)
			files[i] = std::string(properties.rawFilePath + "/" + files[i]);

		for (int i=0; i<files.size(); i++)
		{
			CopyFileToBuffer(files[i], numBytesInBufferFilled, properties);
		}
	}
	else
	{
		CopyFileToBuffer(properties.rawFilePath, numBytesInBufferFilled, properties);
	}
}


// Current file to be copied to buffer. Offset is for multiple files being read into single buffer, set as file offset
void VoxelReader::CopyFileToBuffer(std::string fileName, int &numBytesInBufferFilled, VolumeProperties &properties)
{
	std::streampos size;

	std::ifstream myFile (fileName, std::ios::in|std::ios::binary|std::ios::ate);

	if (myFile.is_open())
	{
		size = myFile.tellg();
		
		myFile.seekg (0, std::ios::beg);
		myFile.read ((char*)properties.bufferAddress + numBytesInBufferFilled, size);
		myFile.close();

		numBytesInBufferFilled += size;
	}
	else 
		std::cout << "Unable to open file";

	
}


// Initializes the values found in VolumeProperties which might not be specified in the header
VolumeProperties::VolumeProperties()
{
	timesteps = 1;
	timePerFrame = 0.1f;
}

