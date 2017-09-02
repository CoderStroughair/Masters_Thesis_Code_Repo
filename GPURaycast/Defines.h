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
#define LOCAL_MAXIMA_COMPUTE_SHADER	"../Shaders/LocalMaximaShader.comp"
#define VISIBILITY_COMPUTE_SHADER	"../Shaders/VisibilityCalculator.comp"

#define VERTEX_SHADER				"../Shaders/VertexShader.vert"
#define FRAGMENT_SHADER				"../Shaders/FragmentShader.frag"

#define TRANS_VERTEX_SHADER			"../Shaders/TransFuncVertShader.vert"
#define	TRANS_FRAGMENT_SHADER		"../Shaders/TransFuncFragShader.frag"
/*----------------------------------------------------------------------------
							VOLUME DATA PATHS
----------------------------------------------------------------------------*/
#define BONSAI_PATH					"../Samples/Bonsai/"
#define BONSAI_HEADER				"../Samples/Bonsai/Bonsai.mhd"

#define	KNEE_PATH					"../Samples/CTknee/"
#define	KNEE_HEADER					"../Samples/CTknee/CTknee.mhd"

#define NUCLEON_PATH				"../Samples/Nucleon/"
#define NUCLEON_HEADER				"../Samples/Nucleon/nucleon.mhd"

#define TOOTH_PATH					"../data/"
#define TOOTH_HEADER				"../data/tooth.mhd"

#define ENGINE_PATH					"../data/"
#define ENGINE_HEADER				"../data/Engine.mhd"

/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
							TRANSFER FUNCTIONS
----------------------------------------------------------------------------*/

#define TRANS_CTKNEE				"../transferfuncs/CT-Knee_spectrum_16_balance.tfi"
#define TRANS_VISMALE				"../transferfuncs/VisMale_spectrum_4_balance_1000.tfi"
#define TRANS_BLUESMOKE				"../transferfuncs/BlueSmoke.tfi"
#define TRANS_NUCLEON				"../transferfuncs/nucleon.tfi"
#define TRANS_MRI_KNEE				"../transferfuncs/MRIKnee.tfi"
#define TRANS_MRI_KNEE_TRANS		"../transferfuncs/MRIKneeTrans.tfi"
#define TRANS_SUPERNOVA				"../transferfuncs/supernova.tfi"
#define TRANS_TEST					"../transferfuncs/test.tfi"
#define TRANS_TEST2					"../transferfuncs/test2.tfi"
#define TRANS_VISIBILITY			"../transferfuncs/visibility.tfi"
#define TRANS_LAPLACIAN				"../transferfuncs/laplacian.tfi"
#define TRANS_TOOTH					"../Samples/Tooth/tooth.tfi"
#define TRANS_TOOTH2				"../data/tooth_balance.tfi"
#define TRANS_TOOTH2LOW				"../data/tooth_balance_low.tfi"
#define TRANS_TOOTH2VLOW			"../data/tooth_balance_verylow.tfi"
#define TRANS_TOOTH2VVLOW			"../data/tooth_balance_veryverylow.tfi"
#define TRANS_ENGINE				"../data/Engine.tfi"

/*--------------------------------------------------------------------------*/

#endif