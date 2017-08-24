#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "TransferFunction.h"
#include "VolumeTexture.h"
#include "EulerCamera.h"
/*----------------------------------------------------------------------------
								DEFINITIONS
----------------------------------------------------------------------------*/

GLuint createOverlayQuad(int location);
GLuint BlankTexture(int tex_w, int tex_h);
void DebugWorkGroups();
void Raycast(TransferFunction transferFunction, GLuint currTexture3D, GLuint shaderProgramID, EulerCamera &camera);
void LaunchComputeShader(GLuint shaderProgramID, GLuint initialTexture3D, GLuint destinationTexture3D, VolumeDataset volume, GLfloat visibilityLowerLimit, GLuint transferFunction);
void LaunchVisibilityComputeShader(VolumeTexture* container, GLuint shaderProgramID, EulerCamera camera, VolumeDataset volume, GLfloat visibilityLowerLimit);

int maxRaySteps = 1000;
float rayStepSize = 0.005f;
float gradientStepSize = 0.005f;
glm::vec3 lightPosition = glm::vec3(-0.0f, -5.0f, 5.0f);
glm::mat4 model_mat = glm::mat4(1.0f);

/*----------------------------------------------------------------------------
								IMPLEMENTATIONS
----------------------------------------------------------------------------*/

GLuint createOverlayQuad(int location) {
	GLuint vao = 0, vbo = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float* verts;
	switch (location)
	{
	case 0:	//Bottom left of screen
		verts = new float[16]
		{
			//positions		//textures
			-1.0f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			-1.0f, -0.5f,	0.0f, 1.0f,	//Top Left
			-0.5f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			-0.5f, -0.5f,	1.0f, 1.0f	//Top Right
		};
		break;
	case 1:	//Bottom Right of screen
		verts = new float[16]
		{
			//positions		//textures
			0.5f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			0.5f, -0.5f,	0.0f, 1.0f,	//Top Left
			1.0f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			1.0f, -0.5f,	1.0f, 1.0f	//Top Right
		};
		break;
	case 2:
		verts = new float[16]
		{
			//positions		//textures
			-1.0f, 0.5f,	0.0f, 0.0f,	//Bottom Left
			-1.0f, 1.0f,	0.0f, 1.0f,	//Top Left
			-0.5f, 0.5f,	1.0f, 0.0f,	//Bottom Right
			-0.5f, 1.0f,	1.0f, 1.0f	//Top Right
		};
		break;
	case 3:
	default:
		verts = new float[16]
		{
			//positions		//textures
			0.5f, 0.5f,	0.0f, 0.0f,	//Bottom Left
			0.5f, 1.0f,	0.0f, 1.0f,	//Top Left
			1.0f, 0.5f,	1.0f, 0.0f,	//Bottom Right
			1.0f, 1.0f,	1.0f, 1.0f	//Top Right
		};
		break;
	case 4:	//Left of Screen
		verts = new float[16]
		{
			//positions		//textures
			-1.0f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			-1.0f, 1.0f,	0.0f, 1.0f,	//Top Left
			0.0f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			0.0f, 1.0f,	1.0f, 1.0f	//Top Right
		};
		break;
	case 5:	//Right of Screen
		verts = new float[16]
		{
			//positions		//textures
			0.0f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			0.0f, 1.0f,		0.0f, 1.0f,	//Top Left
			1.0f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			1.0f, 1.0f,		1.0f, 1.0f	//Top Right
		};
		break;
	case 6:	//Bottom of Screen
		verts = new float[16]
		{
			//positions		//textures
			-0.5f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			-0.5f, -0.5f,	0.0f, 1.0f,	//Top Left
			0.5f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			0.5f, -0.5f,	1.0f, 1.0f	//Top Right
		};
		break;
	}


	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	GLintptr stride = 4 * sizeof(float);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);

	glEnableVertexAttribArray(1);
	GLintptr offset = 2 * sizeof(float);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
	return vao;
}

GLuint BlankTexture(int tex_w, int tex_h)
{
	GLuint tex_output;
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	return tex_output;
}

void DebugWorkGroups()
{
	int work_grp_size[3], work_grp_inv;
	// maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_size[2]);
	printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_size[0],
		work_grp_size[1], work_grp_size[2]);
	// maximum local work group (one shader's slice)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	// maximum compute shader invocations (x * y * z)
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max computer shader invocations %i\n", work_grp_inv);
}

void Raycast(TransferFunction transferFunction, GLuint currTexture3D, GLuint shaderProgramID, EulerCamera &camera)
{
	glUseProgram(shaderProgramID);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "proj"), 1, GL_FALSE, &camera.getProj()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, &camera.getView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, &model_mat[0][0]);

	glUniform3f(glGetUniformLocation(shaderProgramID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	glUniform1i(glGetUniformLocation(shaderProgramID, "maxRaySteps"), maxRaySteps);
	glUniform1f(glGetUniformLocation(shaderProgramID, "rayStepSize"), rayStepSize);
	glUniform1f(glGetUniformLocation(shaderProgramID, "gradientStepSize"), gradientStepSize);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "volume"), 0);
	glBindTexture(GL_TEXTURE_3D, currTexture3D);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shaderProgramID, "transferFunc"), 1);
	glBindTexture(GL_TEXTURE_1D, transferFunction.tfTexture);

	glBegin(GL_QUADS);
	// Front Face
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	// Back Face
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Top Face
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Bottom Face
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Left Face
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_3D, 0);
}

void LaunchComputeShader(GLuint shaderProgramID, GLuint initialTexture3D, GLuint destinationTexture3D, VolumeDataset volume, GLfloat visibilityLowerLimit, GLuint transferFunction)
{
	glUseProgram(shaderProgramID);
	glBindImageTexture(0, initialTexture3D, 0, /*layered=*/GL_TRUE, 0, GL_READ_WRITE, GL_R8);
	glBindImageTexture(1, destinationTexture3D, 0, /*layered=*/GL_TRUE, 0, GL_READ_WRITE, GL_R8);
	glUniform1f(glGetUniformLocation(shaderProgramID, "lowerLimit"), visibilityLowerLimit);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "transferFunc"), 0);
	glBindTexture(GL_TEXTURE_1D, transferFunction);

	GLint localWorkGroupSize[3] = { 0 };
	glGetProgramiv(shaderProgramID, GL_COMPUTE_WORK_GROUP_SIZE, localWorkGroupSize);
	glDispatchCompute((GLuint)volume.xRes / localWorkGroupSize[0], (GLuint)volume.yRes / localWorkGroupSize[1], (GLuint)volume.zRes / localWorkGroupSize[2]);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void LaunchVisibilityComputeShader(VolumeTexture* container, GLuint shaderProgramID, EulerCamera camera, VolumeDataset volume, GLfloat visibilityLowerLimit)
{
	glUseProgram(shaderProgramID);
	glBindImageTexture(0, container->visibilityTexture, 0, /*layered=*/GL_TRUE, 0, GL_READ_WRITE, GL_R8);

	glUniform3f(glGetUniformLocation(shaderProgramID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	glUniform1i(glGetUniformLocation(shaderProgramID, "maxRaySteps"), maxRaySteps);
	glUniform1f(glGetUniformLocation(shaderProgramID, "rayStepSize"), rayStepSize);
	glUniform1f(glGetUniformLocation(shaderProgramID, "lowerLimit"), visibilityLowerLimit);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "volume"), 0);
	glBindTexture(GL_TEXTURE_3D, container->dataTexture);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shaderProgramID, "transferFunc"), 1);
	glBindTexture(GL_TEXTURE_1D, container->dataTF.tfTexture);

	GLint localWorkGroupSize[3] = { 0 };
	glGetProgramiv(shaderProgramID, GL_COMPUTE_WORK_GROUP_SIZE, localWorkGroupSize);
	glDispatchCompute((GLuint)volume.xRes / localWorkGroupSize[0], (GLuint)volume.yRes / localWorkGroupSize[1], (GLuint)volume.zRes / localWorkGroupSize[2]);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
#endif