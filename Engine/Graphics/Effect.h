#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "cShader.h"
#include "cRenderState.h"
#include <Engine/Assets/cbReferenceCountedAsset.h>


#if defined (EAE6320_PLATFORM_D3D)
#include "Direct3D/Includes.h"
#elif defined (EAE6320_PLATFORM_GL)
#include "OpenGL/Includes.h"
#endif
namespace eae6320 {
	class cResult;
}

namespace eae6320_001 {
	class Effect
	{
	private:

#if defined( EAE6320_PLATFORM_GL )
		GLuint s_programId = 0;
#endif

		eae6320::Graphics::cShader::Handle s_vertexShader;
		eae6320::Graphics::cShader::Handle s_fragmentShader;

		eae6320::cResult  InitializeShadingData_GL();

		eae6320::cResult Cleanup_GL();
		
		Effect();

	public:
		eae6320::Graphics::cRenderState s_renderState;
		
		//Effect(const Effect& copy);
		
		EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Effect);
		
		EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
	
		static eae6320::cResult Initialize(Effect*& o_effect);

		eae6320::cResult InitializeShadingData(const char * vertexshader, const char * fragmentShader, const uint8_t renderState = 0);

		void BindShadingData();

			~Effect();

			eae6320::cResult Cleanup();

	};
}
#endif