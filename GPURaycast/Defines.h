#ifndef DEFINES_H
#define DEFINES_H

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
#define COMPUTE_3D_SHADER			"../Shaders/3DComputeShader.comp"
#define LAPLACIAN_COMPUTE_SHADER	"../Shaders/LaplacianShader.comp"
#define GAUSSIAN_COMPUTE_SHADER		"../Shaders/GaussianShader.comp"
#define VISIBILITY_COMPUTE_SHADER	"../Shaders/VisibilityCalculator.comp"

#define VERTEX_SHADER				"../Shaders/VertexShader.vert"
#define FRAGMENT_SHADER				"../Shaders/FragmentShader.frag"

#define TRANS_VERTEX_SHADER			"../Shaders/TransFuncVertShader.vert"
#define	TRANS_FRAGMENT_SHADER		"../Shaders/TransFuncFragShader.frag"
/*----------------------------------------------------------------------------
							VOLUME DATA PATHS
----------------------------------------------------------------------------*/
#define	KNEE_PATH					"../Samples/CTknee/"
#define	KNEE_HEADER					"CTknee.mhd"


/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
							TRANSFER FUNCTIONS
----------------------------------------------------------------------------*/

#define TRANS_CTKNEE			"../transferfuncs/CT-Knee_spectrum_16_balance.tfi"
#define TRANS_VISMALE			"../transferfuncs/VisMale_spectrum_4_balance_1000.tfi"
#define TRANS_BLUESMOKE			"../transferfuncs/BlueSmoke.tfi"
#define TRANS_NUCLEON			"../transferfuncs/nucleon.tfi"
#define TRANS_MRI_KNEE			"../transferfuncs/MRIKnee.tfi"
#define TRANS_MRI_KNEE_TRANS	"../transferfuncs/MRIKneeTrans.tfi"
#define TRANS_SUPERNOVA			"../transferfuncs/supernova.tfi"
#define TRANS_TEST				"../transferfuncs/test.tfi"
#define TRANS_TEST2				"../transferfuncs/test2.tfi"
#define TRANS_VISIBILITY		"../transferfuncs/visibility.tfi"
#define TRANS_LAPLACIAN			"../transferfuncs/laplacian.tfi"

/*--------------------------------------------------------------------------*/


#endif