#include "Shader.h"
#include <iostream>
#include "Defines.h"

#define DEFAULT_VERT	"../Shaders/simpleVertexShader.txt"
#define DEFAULT_FRAG	"../Shaders/simpleFragmentShader.txt"

using namespace std;

Shader* Shader::Instance = 0;

// Create a NULL-terminated string by reading the provided file
char* Shader::readShaderSource(const char* shaderFile) {

	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

Shader* Shader::GetInstance()
{
	if( Instance == 0 )
	{
		Instance = new Shader();
	}
	return Instance;
};

Shader::Shader(){}

void Shader::AddShader( GLuint &shaderProgram, const char* pShaderText, GLenum shaderType )
{
	GLuint shaderObj = glCreateShader( shaderType );
	if( shaderObj == 0 ) {
		fprintf( stderr, "Error creating shader type %d\n", shaderType );
		exit( 1 );
	}
	const char* pShaderSource = readShaderSource( pShaderText );
	glShaderSource( shaderObj, 1, (const GLchar**)&pShaderSource, NULL );
	glCompileShader( shaderObj );
	//CheckStatus(shaderObj);
	checkCompileError( shaderObj, shaderType );
	glAttachShader( shaderProgram, shaderObj );
}

GLuint Shader::CompileShader( char* vertex, char* fragment )
{
	GLuint shaderProgramID = glCreateProgram();
	if( shaderProgramID == 0 )
	{
		fprintf( stderr, "Error creating shader program\n" );
		exit( 1 );
	}
	AddShader( shaderProgramID, vertex, GL_VERTEX_SHADER );
	AddShader( shaderProgramID, fragment, GL_FRAGMENT_SHADER );
	glLinkProgram( shaderProgramID );
	checkLinkError( shaderProgramID );
	glValidateProgram( shaderProgramID );
	checkValidationErrors( shaderProgramID );
	return shaderProgramID;

}

GLuint Shader::CompileComputeShader( char* compute )
{
	GLuint shaderProgramID = glCreateProgram();
	if( shaderProgramID == 0 )
	{
		fprintf( stderr, "Error creating shader program\n" );
		exit( 1 );
	}
	AddShader( shaderProgramID, compute, GL_COMPUTE_SHADER );
	glLinkProgram( shaderProgramID );
	checkLinkError( shaderProgramID );
	glValidateProgram( shaderProgramID );
	checkValidationErrors( shaderProgramID );
	return shaderProgramID;
}

bool Shader::checkCompileError( GLuint shader, GLenum ShaderType )
{
	GLint params = -1;
	GLchar ErrorLog[512] = {0};
	GLint size = 0;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &params );
	if( GL_TRUE != params ) {
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &params );
		glGetShaderInfoLog( shader, 512, &size, ErrorLog );
		fprintf( stderr, "Error compiling shader number %i of type %d: '%s'\n", shader, ShaderType, ErrorLog );
		exit( 1 );
	}
	return true;
}

bool Shader::checkLinkError( GLuint program ) {
	GLint params = -1;
	GLchar ErrorLog[1024] = {0};
	glGetProgramiv( program, GL_LINK_STATUS, &params );
	if( GL_TRUE != params ) {
		glGetProgramInfoLog( program, sizeof( ErrorLog ), NULL, ErrorLog );
		fprintf( stderr, "Linking Error: Invalid shader program: '%s'\n", ErrorLog );
		exit( 1 );
	}
	return true;
}

bool Shader::checkValidationErrors( GLuint program ) {
	GLint params = -1;
	GLchar ErrorLog[1024] = {0};
	glGetProgramiv( program, GL_VALIDATE_STATUS, &params );
	if( GL_TRUE != params )
	{
		glGetProgramInfoLog( program, sizeof( ErrorLog ), NULL, ErrorLog );
		fprintf( stderr, "Validation Error: Invalid shader program: '%s'\n", ErrorLog );
		exit( 1 );
	}
	return true;
}

void Shader::CheckStatus( GLuint obj )
{
	GLint status = GL_FALSE;
	if( glIsShader( obj ) ) glGetShaderiv( obj, GL_COMPILE_STATUS, &status );
	if( glIsProgram( obj ) ) glGetProgramiv( obj, GL_LINK_STATUS, &status );
	if( status == GL_TRUE ) return;
	GLchar log[1 << 15] = {0};
	if( glIsShader( obj ) ) glGetShaderInfoLog( obj, sizeof( log ), NULL, log );
	if( glIsProgram( obj ) ) glGetProgramInfoLog( obj, sizeof( log ), NULL, log );
	std::cerr << log << std::endl;
	exit( EXIT_FAILURE );
}