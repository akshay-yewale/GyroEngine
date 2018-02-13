#include "Sprites.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cSamplerState.h"
#include "cShader.h"
#include "sContext.h"
#include "sSpriteVertexFormat.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <utility>

#if defined (EAE6320_PLATFORM_D3D)
#include "Direct3D\Includes.h"
#elif defined (EAE6320_PLATFORM_GL)
#include "OpenGL\Includes.h"
#endif


//eae6320_001::Sprites::Sprites(const eae6320_001::Sprites& i_copy) :
//	m_lowerbound(i_copy.m_lowerbound),
//	m_width(i_copy.m_width),
//	m_height(i_copy.m_height)
//{
//#if defined( EAE6320_PLATFORM_D3D )
//	
//	 s_vertexBuffer = i_copy.s_vertexBuffer;
//
//	ID3D11InputLayout* s_vertexInputLayout = i_copy.s_vertexInputLayout;
//
//#elif defined( EAE6320_PLATFORM_GL )
//	
//	s_vertexBufferId = i_copy.s_vertexBufferId;
//	
//	s_vertexArrayId = i_copy.s_vertexArrayId;
//#endif
//}

eae6320::cResult eae6320_001::Sprites::Initialize(eae6320_001::Sprites*& o_sprite)
{
	auto result = eae6320::Results::Success;
	Sprites * newsprite = nullptr;

	{
		newsprite = new (std::nothrow) Sprites();
		if (!newsprite) {
			result = eae6320::Results::OutOfMemory;
			goto OnExit;
		}
	}
OnExit:
	if (result)
	{
		EAE6320_ASSERT(newsprite);
		o_sprite = newsprite;
	}
	else
	{
		if (newsprite) {
			newsprite->DecrementReferenceCount();
			newsprite = nullptr;

		}
		o_sprite = nullptr;
	}

	return result;

}