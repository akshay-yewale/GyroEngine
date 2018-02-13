#include "../Mesh.h"
#include "../cConstantBuffer.h"
#include "../ConstantBufferFormats.h"
#include "../cRenderState.h"
#include "../cSamplerState.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../sSpriteVertexFormat.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <utility>
#include <Engine/Results/cResult.h>

#if defined (EAE6320_PLATFORM_D3D)
#include "..\Direct3D\Includes.h"
#elif defined (EAE6320_PLATFORM_GL)
#include "..\OpenGL\Includes.h"
#endif


eae6320_001::Mesh::Mesh() {
	
}

eae6320_001::Mesh::~Mesh() {
	CleanUp();
}

eae6320::cResult eae6320_001::Mesh::InitializeGeometry(eae6320_001::VertexFormat::sMeshVertexDataFormat VertexData[], size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount){
	auto result = eae6320::Results::Success;
	
	
	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &s_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(s_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &s_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, s_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	//create a index buffer and make it active
	{
		constexpr GLsizei indexbufferCount = 1;
		glGenBuffers(indexbufferCount, &m_indexBuffer);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	
	//Assign Data to Index buffer
	{
		
		const auto indexbufferSize = IndexDataCount * sizeof(uint16_t);
		m_indexCount = IndexDataCount;
		EAE6320_ASSERT(indexbufferSize < (uint64_t(1u) << (sizeof(GLsizeiptr) * 8)));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexbufferSize), reinterpret_cast<GLvoid*>(IndexData),
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	//Assigning data to the buffer
	{
	
		const auto bufferSize = VertexDataCount * sizeof(eae6320_001::VertexFormat::sMeshVertexDataFormat);
		EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(GLsizeiptr) * 8)));
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(VertexData),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		const auto stride = static_cast<GLsizei>(sizeof(eae6320_001::VertexFormat::sMeshVertexDataFormat));

		// Position (0)
		// 2 floats == 8 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			//constexpr GLint elementCount = 2;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, x)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

		// For Color Cordinates
		{
			constexpr GLuint _vertexElementLocation = 1;
			constexpr GLint elementCount = 4;
			constexpr GLboolean notNormalized = GL_TRUE;// should be true for normalized	// The given floats should be used as-is
			glVertexAttribPointer(_vertexElementLocation, elementCount, GL_UNSIGNED_BYTE , notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, r)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(_vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						_vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					_vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

		//For Texture Cordinates
		{
			constexpr GLuint _vertexElementLocation = 2;
			constexpr GLint elementCount = 2;
			constexpr GLboolean notNormalized = GL_FALSE; // should be true for normalized	// The given floats should be used as-is
			glVertexAttribPointer(_vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, u)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(_vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						_vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					_vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

	}
	OnExit:

	return result;
}

void eae6320_001::Mesh::Draw() {
	{
		

		// Bind a specific vertex buffer to the device as a data source
		{
			glBindVertexArray(s_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Render triangles from the currently-bound vertex buffer
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// a triangle list is defined
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		constexpr GLenum mode = GL_TRIANGLES;
		
		// It's possible to start rendering primitives in the middle of the stream
		const GLvoid* const offset = 0;
		glDrawElements(mode, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, offset);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

eae6320::cResult eae6320_001::Mesh::CleanUp() {
	auto result = eae6320::Results::Success;

	if (s_vertexArrayId != 0)
	{
		// Make sure that the vertex array isn't bound
		{
			// Unbind the vertex array
			glBindVertexArray(0);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = eae6320::Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
		}
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexArrayId = 0;
	}
	if (s_vertexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexBufferId = 0;
	}
	if (m_indexBuffer != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_indexCount);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_indexCount = 0;
	}
	return result;
}

/*
eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh, eae6320_001::VertexFormat::sMeshVertexFormat VertexData[], size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount)
{
	auto result = eae6320::Results::Success;
	Mesh * newsprite = nullptr;

	{
		newsprite = new (std::nothrow) Mesh();
		if (!newsprite) {
			result = eae6320::Results::OutOfMemory;
			goto OnExit;
		}
	}
OnExit:
	if (result)
	{
		EAE6320_ASSERT(newsprite);
		o_mesh = newsprite;
		

		if (!(result = o_mesh->InitializeGeometry(VertexData, VertexDataCount, IndexData, IndexDataCount))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}


	}
	else
	{
		if (newsprite) {
			newsprite->DecrementReferenceCount();
			newsprite = nullptr;

		}
		o_mesh = nullptr;
	}

	return result;

}
*/
eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh, eae6320_001::VertexFormat::sMeshVertexDataFormat VertexData[], size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount)
{
	auto result = eae6320::Results::Success;
	Mesh * newsprite = nullptr;

	{
		newsprite = new (std::nothrow) Mesh();
		if (!newsprite) {
			result = eae6320::Results::OutOfMemory;
			goto OnExit;
		}
	}
OnExit:
	if (result)
	{
		EAE6320_ASSERT(newsprite);
		o_mesh = newsprite;


		if (!(result = o_mesh->InitializeGeometry(VertexData, VertexDataCount, IndexData, IndexDataCount))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}


	}
	else
	{
		if (newsprite) {
			newsprite->DecrementReferenceCount();
			newsprite = nullptr;

		}
		o_mesh = nullptr;
	}

	return result;

}

