#include "..\GraphicsAbstractionLayer.h"
#include "Includes.h"

#include <Engine\Asserts\Asserts.h>
#include <Engine\Graphics\sContext.h>
#include <Engine\Logging\Logging.h>


eae6320::cResult eae6320_001::cGraphicsAbstractionLayer::InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
{
	return eae6320::Results::Success;
}

void eae6320_001::cGraphicsAbstractionLayer::CleanUp(){
}

void eae6320_001::cGraphicsAbstractionLayer::SetClearColor(const float clearColor[4]) {

	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	constexpr GLbitfield clearColorBufferBit = GL_COLOR_BUFFER_BIT;
	glClear(clearColorBufferBit);
	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
}

void eae6320_001::cGraphicsAbstractionLayer::SwapDisplayBuffers()
{
	const auto deviceContext = eae6320::Graphics::sContext::g_context.deviceContext;
	EAE6320_ASSERT(deviceContext != NULL);

	const auto glResult = SwapBuffers(deviceContext);
	EAE6320_ASSERT(glResult != FALSE);
}

void eae6320_001::cGraphicsAbstractionLayer::ClearDepthBuffer(float index) {
	{
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		glClearDepth(index);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
		glClear(clearDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}