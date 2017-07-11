#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "TransferFunction.h"
#include "EulerCamera.h"
/*----------------------------------------------------------------------------
								DEFINITIONS
----------------------------------------------------------------------------*/

GLuint quadVAO();
GLuint BlankTexture(int tex_w, int tex_h);
void DebugWorkGroups();
void Raycast(GLuint currTexture3D, TransferFunction &transferFunction, GLuint shaderProgramID, EulerCamera &camera);

/*----------------------------------------------------------------------------
								IMPLEMENTATIONS
----------------------------------------------------------------------------*/

GLuint quadVAO() {
	GLuint vao = 0, vbo = 0;
	float verts[] = 
	{ 
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
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
	// linear allows us to scale the window up retaining reasonable quality
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// same internal format as compute shader input
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
		NULL);
	// bind to image unit so can write to specific pixels from the shader
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
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

void Raycast(GLuint currTexture3D, TransferFunction &transferFunction, GLuint shaderProgramID, EulerCamera &camera)
{
	glUseProgram(shaderProgramID);
	int maxRaySteps = 1000;
	float rayStepSize = 0.005f;
	float gradientStepSize = 0.005f;
	glm::vec3 lightPosition = glm::vec3(-0.0f, -5.0f, 5.0f);
	glm::mat4 model_mat = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "proj"), 1, GL_FALSE, glm::value_ptr(camera.getProj()));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(camera.getView()));
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


#endif