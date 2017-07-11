#include "TransferFunction.h"

void TransferFunction::Init(const char *filename, VolumeDataset &volume_)
{
	// Want the transfer functions to have 256 possible values
	colorTable.resize(256);
	LoadXML(filename);

	colors.resize(numIntensities);

	//Generate the 1D texture
	glGenTextures(1, &tfTexture);
    glBindTexture(GL_TEXTURE_1D, tfTexture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, 0); 
	glBindTexture(GL_TEXTURE_1D, 0);

	LoadLookup(colorTable);
}



// Reads XML file transfer function and copies it into the tables
void TransferFunction::LoadXML(const char *filename)
{
	tinyxml2::XMLDocument doc;
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/CT-Knee_spectrum_16_balance.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/VisMale_spectrum_4_balance_1000.tfi");
	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/BlueSmoke.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/nucleon.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/MRIKnee.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/MRIKneeTrans.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/supernova.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/test.tfi");
//	tinyxml2::XMLError r = doc.LoadFile("../transferfuncs/test2.tfi");

	if (r != tinyxml2::XML_NO_ERROR)
	{
		fprintf(stderr, "failed to open file");
		exit(404);
	}

	tinyxml2::XMLElement* transFuncIntensity = doc.FirstChildElement("VoreenData")->FirstChildElement("TransFuncIntensity");

	tinyxml2::XMLElement* key = doc.FirstChildElement("VoreenData")->FirstChildElement("TransFuncIntensity")->FirstChildElement("Keys")->FirstChildElement("key");

	while (key)
	{
		float intensity = atof(key->FirstChildElement("intensity")->Attribute("value"));
		intensities.push_back(intensity);

		int r = atoi(key->FirstChildElement("colorL")->Attribute("r"));
		int g = atoi(key->FirstChildElement("colorL")->Attribute("g"));
		int b = atoi(key->FirstChildElement("colorL")->Attribute("b"));
		int a = atoi(key->FirstChildElement("colorL")->Attribute("a"));

		colors.push_back(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));

		key = key->NextSiblingElement();
	}

	numIntensities = intensities.size();
}

// Because file only specifies a number of control points, a full color table must be made from these control points by linearly interpolating
void TransferFunction::LoadLookup(std::vector<glm::vec4> &colorTable)
{
	glm::vec4 previousColor(0.0f);
	float previousIntensity = 0.0f;
	int next = 0;

	for (int i=0; i<256; i++)
	{
		float currentIntensity = (float)i / (float)255;

		while (next < numIntensities && currentIntensity > intensities[next])
		{
			previousIntensity = intensities[next];
			previousColor = colors[next];
			next++;
		}

		if (next < numIntensities)
			colorTable[i] = LERPColor(previousColor, colors[next], previousIntensity, intensities[next], currentIntensity);
		else
			colorTable[i] = LERPColor(previousColor, glm::vec4(0.0f), previousIntensity, 1.0f, currentIntensity);
	}

	CopyToTex(colorTable);
}

// Copy final table to GPU memory
void TransferFunction::CopyToTex(std::vector<glm::vec4> &data)
{
	glBindTexture(GL_TEXTURE_1D, tfTexture);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, &data[0]);
	glBindTexture(GL_TEXTURE_1D, 0);
}


// Linearly interpolate between two control points
glm::vec4 TransferFunction::LERPColor(glm::vec4 firstColor, glm::vec4 secondColor, float firstIntensity, float secondIntensity, float currentIntensity)
{
	float difference = secondIntensity - firstIntensity;

	if (difference > 0.0f)
	{
		float fraction = (currentIntensity - firstIntensity) / difference;

		return firstColor + ((secondColor - firstColor) * fraction);
	}
	else
		return firstColor;
}
