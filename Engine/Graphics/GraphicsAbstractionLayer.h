#ifndef _GRAPHICS_ABSTRACTION_LAYER_H
#define _GRAPHICS_ABSTRACTION_LAYER_H

#include "Graphics.h"

class eae6320::cResult;
namespace eae6320_001 {
	

	class cGraphicsAbstractionLayer {

	public:
		eae6320::cResult InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);

		void CleanUp();

		void SetClearColor(const float clearColor[4]);

		void SwapDisplayBuffers();

		void ClearDepthBuffer(float depthClearIndex = 1.0f );
	};
}
#endif