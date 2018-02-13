#ifndef _MESH_H
#define _MESH_H

#if defined (EAE6320_PLATFORM_D3D)
#include "Direct3D/Includes.h"
#elif defined (EAE6320_PLATFORM_GL)
#include "OpenGL/Includes.h"
#endif

#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Results/Results.h>
#include <stdint.h>
#include <array>
#include "sSpriteVertexFormat.h"
#include <External/Lua/Includes.h>

#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>

namespace eae6320_001 {
	class Mesh
	{

#if defined( EAE6320_PLATFORM_D3D )
		// A vertex buffer holds the data for each vertex
		ID3D11Buffer* s_vertexBuffer = nullptr;
		// D3D has an "input layout" object that associates the layout of the vertex format struct
		// with the input from a vertex shader
		ID3D11InputLayout* s_vertexInputLayout = nullptr;
		// A index vertex  buffer holds the data for each vertex
		ID3D11Buffer* m_indexBuffer = nullptr;

#elif defined( EAE6320_PLATFORM_GL )
		// A vertex buffer holds the data for each vertex
		GLuint s_vertexBufferId = 0;
		// A vertex array encapsulates the vertex data as well as the vertex input layout
		GLuint s_vertexArrayId = 0;
		// Index buffer 
		GLuint m_indexBuffer = 0;
#endif

	public:
		size_t m_indexCount;

		uint16_t * IndexData;

		size_t m_VertexDataCount;

		eae6320_001::VertexFormat::sMeshVertexDataFormat * VertexData;

		using Handle = eae6320::Assets::cHandle<Mesh>;

		static eae6320::Assets::cManager<Mesh> s_manager;

		static eae6320::cResult Load(const char* const i_path, Mesh*& o_mesh);

		Mesh();

		~Mesh();

		EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh);

		EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

		eae6320::cResult InitializeGeometry(eae6320_001::VertexFormat::sMeshVertexDataFormat VertexData[],
			size_t VertexDataCount, uint16_t IndexData[], size_t IndexDataCount);

		void Draw();

		eae6320::cResult CleanUp();

		static eae6320::cResult Initialize(eae6320_001::Mesh *& o_Mesh,
			eae6320_001::VertexFormat::sMeshVertexDataFormat VertexData[],
			size_t VertexDataCount, uint16_t IndexData[], size_t IndexDataCount);

		static eae6320::cResult Initialize(eae6320_001::Mesh *& o_Mesh,	const char * meshfile);

		static eae6320::cResult Initialize(eae6320_001::Mesh *&o_Mesh);

		eae6320::cResult LoadDataFromFile(const char * i_path);
		
		/*
#pragma region LUA_FUNCTIONS

		eae6320::cResult LoadAsset(const char* const i_path);
	
		eae6320::cResult LoadTableValues(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_VertexData(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_VertexData_Values(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_IndexData(lua_State& io_luaState);

		eae6320::cResult eae6320_001::Mesh::LoadTableValues_IndexData_Values(lua_State& io_luaState);

		eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_Position(lua_State& io_luaState
		, eae6320_001::VertexFormat::sMeshVertexDataFormat &o);

		eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_Color(lua_State& io_luaState,
			eae6320_001::VertexFormat::sMeshVertexDataFormat &o);

		eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_UV(lua_State& io_luaState,
			eae6320_001::VertexFormat::sMeshVertexDataFormat &o);

#pragma endregion LUA_FUNCTIONS
*/
	};
}
#endif
