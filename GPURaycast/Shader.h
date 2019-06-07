#pragma once

#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>
#include <iostream>

#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

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
private:

	static Shader* Instance;
	Shader();

	void AddShader( GLuint &ShaderProgram, const char* pShaderText, GLenum ShaderType );
	char* readShaderSource( const char* shaderFile );
	bool checkCompileError( GLuint shader, GLenum ShaderType );
	bool checkLinkError( GLuint shader );
	bool checkValidationErrors( GLuint program );
	static void CheckStatus( GLuint obj );

public:
	GLuint CompileShader(char* vertex, char* fragment);
	GLuint CompileComputeShader(char* compute);
	static Shader* GetInstance();
};