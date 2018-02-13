#include "Effect.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cSamplerState.h"
#include "sContext.h"


#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <utility>


eae6320_001::Effect::~Effect(){
	Cleanup();
}

eae6320_001::Effect::Effect(){
}

//eae6320_001::Effect::Effect(const eae6320_001::Effect& i_copy):
//	s_vertexShader(i_copy.s_vertexShader),
//	s_fragmentShader(i_copy.s_fragmentShader),
//	s_renderState(i_copy.s_renderState)
//	//m_referenceCount(i_copy.m_referenceCount)
//{
//	#if defined EAE6320_PLATFORM_GL
//	s_programId = i_copy.s_programId;
//#endif
//}

eae6320::cResult eae6320_001::Effect::InitializeShadingData_GL()
{
	auto result = eae6320::Results::Success;

#if defined( EAE6320_PLATFORM_GL )
	// Create a program
	{
		s_programId = glCreateProgram();
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
		else if (s_programId == 0)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to create a program");
			goto OnExit;
		}
	}
	// Attach the shaders to the program
	{
		// Vertex
		{
			glAttachShader(s_programId, eae6320::Graphics::cShader::s_manager.Get(s_vertexShader)->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		// Fragment
		{
			glAttachShader(s_programId, eae6320::Graphics::cShader::s_manager.Get(s_fragmentShader)->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}
	// Link the program
	{
		glLinkProgram(s_programId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoSize);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					struct sLogInfo
					{
						GLchar* memory;
						sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
						~sLogInfo() { if (memory) free(memory); }
					} info(static_cast<size_t>(infoSize));
					GLsizei* const dontReturnLength = nullptr;
					glGetProgramInfoLog(s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					const auto errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						linkInfo = info.memory;
					}
					else
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(s_programId, GL_LINK_STATUS, &didLinkingSucceed);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, linkInfo.c_str());
						eae6320::Logging::OutputError("The program failed to link: %s",
							linkInfo.c_str());
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:

	if (!result)
	{
		if (s_programId != 0)
		{
			glDeleteProgram(s_programId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_programId = 0;
		}
	}
#endif

	return result;
}

eae6320::cResult eae6320_001::Effect::InitializeShadingData(const char * vertexshader, const char * fragmentShader, const uint8_t renderState) {

	auto result = eae6320::Results::Success;
	char * vShader = new char[100];
	strcpy(vShader, "data/Shaders/Vertex/");
	strcat(vShader, vertexshader);
	strcat(vShader, ".shd");
	if (!(result = eae6320::Graphics::cShader::s_manager.Load(vShader,
		s_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERT(false);
		result = eae6320::Results::Failure;
		goto OnExit;
	}

	char*  name = new char[100];
	strcpy(name, "data/Shaders/Fragment/");
	strcat(name, fragmentShader);
	strcat(name, ".shd");
	//std::string name;
	//std::string relativeAddress("data/Shaders/Fragment/");
	//std::string extension(".shd");
	//std::string name = "data/Shaders/Fragment/" + fragmentShader + ".shd";
	if (!(result = eae6320::Graphics::cShader::s_manager.Load(name,
		s_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment)))
	{
		EAE6320_ASSERT(false);
		result = eae6320::Results::Failure;
		goto OnExit;
	}


	{
		//constexpr uint8_t defaultRenderState = renderState;
		
		if (!(result = s_renderState.Initialize(renderState)))
		{
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

	}
	
#if defined (EAE6320_PLATFORM_GL)
	result = InitializeShadingData_GL();
#endif // (EAE6320_PLATFORM_GL)

OnExit:
	return result;

}

void eae6320_001::Effect::BindShadingData() {

#if defined( EAE6320_PLATFORM_D3D )
	{
		{
			auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;

			ID3D11ClassInstance* const* noInterfaces = nullptr;
			constexpr unsigned int interfaceCount = 0;
			// Vertex shader
			{
				EAE6320_ASSERT(s_vertexShader);
				auto* const shader = eae6320::Graphics::cShader::s_manager.Get(s_vertexShader);
				EAE6320_ASSERT(shader && shader->m_shaderObject.vertex);
				direct3dImmediateContext->VSSetShader(shader->m_shaderObject.vertex, noInterfaces, interfaceCount);
			}
			// Fragment shader
			{
				EAE6320_ASSERT(s_fragmentShader);
				auto* const shader = eae6320::Graphics::cShader::s_manager.Get(s_fragmentShader);
				EAE6320_ASSERT(shader && shader->m_shaderObject.fragment);
				direct3dImmediateContext->PSSetShader(shader->m_shaderObject.fragment, noInterfaces, interfaceCount);
			}
		}
		s_renderState.Bind();
	}
#elif defined( EAE6320_PLATFORM_GL )
	{
		{
			EAE6320_ASSERT(s_programId != 0);
			glUseProgram(s_programId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		s_renderState.Bind();
	}

#endif 
}

eae6320::cResult eae6320_001::Effect::Cleanup() {
	auto result = eae6320::Results::Success;

#if defined (EAE6320_PLATFORM_GL)
	result = Cleanup_GL();
#endif
	
	if (s_vertexShader)
	{
		const auto localResult = eae6320::Graphics::cShader::s_manager.Release(s_vertexShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	if (s_fragmentShader)
	{
		const auto localResult = eae6320::Graphics::cShader::s_manager.Release(s_fragmentShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_renderState.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	return result;
}

//eae6320_001::Effect::Effect(const Effect & copy)
//{
//}

eae6320::cResult eae6320_001::Effect::Cleanup_GL() {
	auto result = eae6320::Results::Success;
#if defined (EAE6320_PLATFORM_GL)
	if (s_programId != 0)
	{
		glDeleteProgram(s_programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_programId = 0;
	}
#endif
	return result;
}

eae6320::cResult eae6320_001::Effect::Initialize(eae6320_001::Effect*& o_effect){
	auto result = eae6320::Results::Success;
	Effect * newEffect = nullptr;

	{
		newEffect = new (std::nothrow) Effect();
		if (!newEffect)
		{
			result = eae6320::Results::OutOfMemory;
			goto OnExit;
		}
	}
	
	OnExit:
	if (result)
	{
		EAE6320_ASSERT(newEffect);
		o_effect = newEffect;
	}
	else
	{
		if (newEffect) {
			newEffect->DecrementReferenceCount();
			newEffect = nullptr;

		}
		o_effect = nullptr;
	}

	return result;
}