#pragma once

#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>

/**
Requirements:
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Init() Function:
shaderProgramID = basic.CompileShader(vert, frag);

If using Compute Shaders, call CompileComputeShader(compute) instead.
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
Then do all your regular mumbo jumbo inside your display file (linking uniforms and such)
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

This class was originally created by following the tutorial steps outlined on learnopengl.com
**/
class Shader 
{
public:
	GLuint CompileShader(char* vertex, char* fragment);
	GLuint CompileComputeShader(char* compute);

private:
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	char* readShaderSource(const char* shaderFile);
	bool checkCompileError(GLuint shader, GLenum ShaderType);
	bool checkLinkError(GLuint shader);
	bool checkValidationErrors(GLuint program);

};

char* Shader::readShaderSource(const char* shaderFile) 
{
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) 
	{
		fprintf(stderr, "Error: Shader %s not found", shaderFile);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

void Shader::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	glCompileShader(ShaderObj);
	checkCompileError(ShaderObj, ShaderType);
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Shader::CompileShader(char* vertex, char* fragment) 
{
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) 
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}
	AddShader(shaderProgramID, vertex, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, fragment, GL_FRAGMENT_SHADER);
	glLinkProgram(shaderProgramID);
	checkLinkError(shaderProgramID);
	glValidateProgram(shaderProgramID);
	checkValidationErrors(shaderProgramID);
	return shaderProgramID;

}

GLuint Shader::CompileComputeShader(char* compute)
{
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) 
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}
	AddShader(shaderProgramID, compute, GL_COMPUTE_SHADER);
	glLinkProgram(shaderProgramID);
	checkLinkError(shaderProgramID);
	glValidateProgram(shaderProgramID);
	checkValidationErrors(shaderProgramID);
	return shaderProgramID;
}

bool Shader::checkCompileError(GLuint shader, GLenum ShaderType)
{
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		glGetProgramInfoLog(shader, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, ErrorLog);
		exit(1);
	}
	return true;
}

bool Shader::checkLinkError(GLuint program) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Linking Error: Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	return true;
}

bool Shader::checkValidationErrors(GLuint program) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	if (GL_TRUE != params)
	{
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Validation Error: Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	return true;
}