#pragma once

#define GOLD	vec3(247.0f / 255.0f, 194.0f / 255.0f, 87.0f / 255.0f)
#define BROWN	vec3(51.0f / 255.0f, 25.0f / 255.0f, 0.0f / 255.0f)
#define BLACK	vec3(0.0f, 0.0f, 0.0f)
#define WHITE	vec3(1.0f, 1.0f, 1.0f)
#define CYAN	vec3(0.0f, 1.0f, 1.0f)
#define MAGENTA	vec3(1.0f, 0.0f, 1.0f)
#define YELLOW	vec3(1.0f, 1.0f, 0.0f)
#define BLUE	vec3(0.0f, 0.0f, 1.0f)
#define RED		vec3(1.0f, 0.0f, 0.0f)
#define GREY	vec3(0.7f, 0.7f, 0.7f)
#define GREEN	vec3(0.0f, 1.0f, 0.0f)
#define PURPLE	vec3(102.0f/255.0f, 0.0, 204.0f/255.0f)

/*----------------------------------------------------------------------------
							SHADER VARIABLES
----------------------------------------------------------------------------*/
#define RAY_COMPUTE_SHADER			"../Shaders/ComputeShader.comp"
#define INVERSE_COMPUTE_SHADER		"../Shaders/InverseComputeShader.comp"

#define VERTEX_SHADER				"../Shaders/VertexShader.vert"
#define FRAGMENT_SHADER				"../Shaders/FragmentShader.frag"
/*----------------------------------------------------------------------------
							VOLUME DATA PATHS
----------------------------------------------------------------------------*/
#define	KNEE_PATH					"../Samples/CTknee/"
#define	KNEE_HEADER					"CTknee.mhd"


/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/