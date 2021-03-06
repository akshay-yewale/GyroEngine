#include "..\Mesh.h"

#include "..\sContext.h"


#include <Engine/Asserts/Asserts.h>
//#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
//#include <Engine/Time/Time.h>
#include <utility>
#include <Engine/Results/cResult.h>

#include "..\Direct3D\Includes.h"

eae6320_001::Mesh::Mesh() {

}

eae6320_001::Mesh::~Mesh() {
	CleanUp();
}

//
//eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh, eae6320_001::VertexFormat::sMeshVertexFormat VertexData[], size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount)
//
//{
//	auto result = eae6320::Results::Success;
//	Mesh * newMesh = nullptr;
//
//	{
//		newMesh = new (std::nothrow) Mesh();
//		if (!newMesh) {
//			result = eae6320::Results::OutOfMemory;
//			goto OnExit;
//		}
//	}
//OnExit:
//	if (result)
//	{
//		EAE6320_ASSERT(newMesh);
//		o_mesh = newMesh;
//		o_mesh->InitializeGeometry(VertexData, VertexDataCount, IndexData, IndexDataCount);
//	}
//	else
//	{
//		if (newMesh) {
//			newMesh->DecrementReferenceCount();
//			newMesh = nullptr;
//
//		}
//		o_mesh = nullptr;
//	}
//
//	return result;
//
//}

eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh, eae6320_001::VertexFormat::sMeshVertexDataFormat VertexData[], size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount)

{
	auto result = eae6320::Results::Success;
	Mesh * newMesh = nullptr;

	{
		newMesh = new (std::nothrow) Mesh();
		if (!newMesh) {
			result = eae6320::Results::OutOfMemory;
			goto OnExit;
		}
	}
OnExit:
	if (result)
	{
		EAE6320_ASSERT(newMesh);
		o_mesh = newMesh;
		o_mesh->InitializeGeometry(VertexData, VertexDataCount, IndexData, IndexDataCount);
	}
	else
	{
		if (newMesh) {
			newMesh->DecrementReferenceCount();
			newMesh = nullptr;

		}
		o_mesh = nullptr;
	}

	return result;

}

void eae6320_001::Mesh::Draw() {
	{
		auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(s_vertexBuffer);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			//constexpr unsigned int bufferStride = sizeof(eae6320_001::VertexFormat::sMeshVertexFormat);
			constexpr unsigned int bufferStride = sizeof(eae6320_001::VertexFormat::sMeshVertexDataFormat);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &s_vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Set the layout (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(s_vertexInputLayout);
				direct3dImmediateContext->IASetInputLayout(s_vertexInputLayout);
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		{
			EAE6320_ASSERT(m_indexBuffer);
			// The indices start at the beginning of the buffer
			const unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, offset);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// As of this comment only a single triangle is drawn
			// (you will have to update this code in future assignments!)
			//constexpr unsigned int triangleCount = 2;
			//constexpr unsigned int vertexCountPerTriangle = 3;
			//constexpr auto vertexCountToRender = triangleCount * vertexCountPerTriangle;
			// It's possible to start rendering primitives in the middle of the stream
			const unsigned int indexOfFirstIndexToUse = 0;
			const unsigned int offsetToAddToEachIndex = 0;
			direct3dImmediateContext->DrawIndexed(static_cast<unsigned int>(m_indexCount), indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}
	}
}

//eae6320::cResult eae6320_001::Mesh::InitializeGeometry(eae6320_001::VertexFormat::sMeshVertexFormat * VertexData , size_t VertexDataCount, uint16_t  IndexData[] , size_t IndexDataCount)
eae6320::cResult eae6320_001::Mesh::InitializeGeometry(eae6320_001::VertexFormat::sMeshVertexDataFormat * VertexData, 
	size_t VertexDataCount, uint16_t  IndexData[], size_t IndexDataCount)

{
		//sMeshVertexInputLayout
		auto result = eae6320::Results::Success;
		//setting the index buffer in left hand winding order
		
		
		auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
		EAE6320_ASSERT(direct3dDevice);

		{
			// Load the compiled binary vertex shader for the input layout
			eae6320::Platform::sDataFromFile vertexShaderDataFromFile;
			std::string errorMessage;
			//if (result = eae6320::Platform::LoadBinaryFile("data/Shaders/Vertex/sMeshVertexInputLayout.shd", vertexShaderDataFromFile, &errorMessage))
			if (result = eae6320::Platform::LoadBinaryFile("data/Shaders/Vertex/sMeshVertexInputLayout_001.shd", vertexShaderDataFromFile, &errorMessage))
			{
				// Create the vertex layout

				// These elements must match the VertexFormats::sGeometry layout struct exactly.
				// They instruct Direct3D how to match the binary data in the vertex buffer
				// to the input elements in a vertex shader
				// (by using so-called "semantic" names so that, for example,
				// "POSITION" here matches with "POSITION" in shader code).
				// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.

				//changes for assignment 11 ie texture build
				constexpr unsigned int vertexElementCount = 3; // 1;
				D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = {};
				{
					// Slot 0

					// POSITION
					// 2 floats == 8 bytes
					// Offset = 0
					// DXGI_FORMAT_R32G32B32_FLOAT is used for 3 floats
					{
						auto& positionElement = layoutDescription[0];

						positionElement.SemanticName = "POSITION";
						positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
						//positionElement.Format = DXGI_FORMAT_R32G32_FLOAT;
						positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
						positionElement.InputSlot = 0;
						//positionElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexFormat, x);
						positionElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, x);
						positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
					}
					{
						auto& colorElement = layoutDescription[1];

						colorElement.SemanticName = "COLOR";
						colorElement.SemanticIndex = 0;
						colorElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						colorElement.InputSlot = 0;
						//textureElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexFormat, r);
						colorElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, r);
						colorElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						colorElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
					}
					{
						auto& textureElement = layoutDescription[2];

						textureElement.SemanticName = "TEXCOORD";
						textureElement.SemanticIndex = 0;
						textureElement.Format = DXGI_FORMAT_R32G32_FLOAT;
						textureElement.InputSlot = 0;
						//textureElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexFormat, r);
						textureElement.AlignedByteOffset = offsetof(eae6320_001::VertexFormat::sMeshVertexDataFormat, u);
						textureElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						textureElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
					}
				}

				const auto d3dResult = direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
					vertexShaderDataFromFile.data, vertexShaderDataFromFile.size, &s_vertexInputLayout);
				if (FAILED(result))
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, "Geometry vertex input layout creation failed (HRESULT %#010x)", d3dResult);
					eae6320::Logging::OutputError("Direct3D failed to create the geometry vertex input layout (HRESULT %#010x)", d3dResult);
				}
				vertexShaderDataFromFile.Free();
			}
			else
			{
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("The geometry vertex input layout shader couldn't be loaded: %s", errorMessage.c_str());
				goto OnExit;
			}
		}

		{
		size_t vertexCount = VertexDataCount;
		//size_t vertexCount = sizeof(VertexData) / sizeof(eae6320_001::VertexFormat::sMeshVertexFormat);

			{
				D3D11_BUFFER_DESC bufferDescription{};
				{
					//const auto bufferSize = vertexCount * sizeof(eae6320_001::VertexFormat::sMeshVertexFormat);
					const auto bufferSize = vertexCount * sizeof(eae6320_001::VertexFormat::sMeshVertexDataFormat);
					EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(bufferDescription.ByteWidth) * 8)));
					bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
					bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
					bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
					bufferDescription.MiscFlags = 0;
					bufferDescription.StructureByteStride = 0;	// Not used
				}
				D3D11_SUBRESOURCE_DATA initialData{};
				{
					initialData.pSysMem = VertexData;
					// (The other data members are ignored for non-texture buffers)
				}

				const auto d3dResult = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_vertexBuffer);
				if (FAILED(d3dResult))
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, "Geometry vertex buffer creation failed (HRESULT %#010x)", d3dResult);
					eae6320::Logging::OutputError("Direct3D failed to create a geometry vertex buffer (HRESULT %#010x)", d3dResult);
					goto OnExit;
				}
			}
			//creation of index buffer
			{
				const auto indexCount = IndexDataCount;
				m_indexCount = IndexDataCount;
				D3D11_BUFFER_DESC bufferDescription{};
				{
					const auto bufferSize = indexCount * sizeof(uint16_t);
					EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(bufferDescription.ByteWidth) * 8)));
					bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
					bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
					bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
					bufferDescription.MiscFlags = 0;
					bufferDescription.StructureByteStride = 0;	// Not used
				}

				D3D11_SUBRESOURCE_DATA initialData{};
				{
					initialData.pSysMem = IndexData;
					// (The other data members are ignored for non-texture buffers)
				}

				const auto d3dResult = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_indexBuffer);
				if (FAILED(d3dResult))
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, "Geometry vertex buffer creation failed (HRESULT %#010x)", d3dResult);
					eae6320::Logging::OutputError("Direct3D failed to create a geometry vertex buffer (HRESULT %#010x)", d3dResult);
					goto OnExit;
				}
			}
		}

	OnExit:
		return result;
}

eae6320::cResult eae6320_001::Mesh::CleanUp() {
	auto result = eae6320::Results::Success;
	if (s_vertexBuffer)
	{
		s_vertexBuffer->Release();
		s_vertexBuffer = nullptr;
	}
	if (s_vertexInputLayout)
	{
		s_vertexInputLayout->Release();
		s_vertexInputLayout = nullptr;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	return result;
}

