#ifndef _SPRITES_H
#define _SPRITES_H
#pragma once
#include "sSpriteVertexFormat.h"
#include <Engine/Assets/ReferenceCountedAssets.h>
//#include <Engine/Results/Results.h>

#if defined (EAE6320_PLATFORM_D3D)
#include "Direct3D/Includes.h"
#elif defined (EAE6320_PLATFORM_GL)
#include "OpenGL/Includes.h"
#endif
namespace eae6320 {
	class cResult;
}
//struct eae6320_001::VertexFormat::sSpriteVertexFormat;

namespace eae6320_001 {
	class Sprites {
	private:
	
		Sprites();
		~Sprites();

#if defined( EAE6320_PLATFORM_D3D )
		// A vertex buffer holds the data for each vertex
		ID3D11Buffer* s_vertexBuffer = nullptr;
		// D3D has an "input layout" object that associates the layout of the vertex format struct
		// with the input from a vertex shader
		ID3D11InputLayout* s_vertexInputLayout = nullptr;

#elif defined( EAE6320_PLATFORM_GL )
		// A vertex buffer holds the data for each vertex
		GLuint s_vertexBufferId = 0;
		// A vertex array encapsulates the vertex data as well as the vertex input layout
		GLuint s_vertexArrayId = 0;
#endif

	public:
		EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Sprites);

		EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

		eae6320::cResult InitializeGeometry(eae6320_001::VertexFormat::sSpriteVertexFormat lowerbound = eae6320_001::VertexFormat::sSpriteVertexFormat(0.0, 0.0,0.0,0.0), float width = 1.0f, float height = 1.0f);

		void Draw();

		eae6320::cResult CleanUp();

		static eae6320::cResult Initialize(Sprites*& o_sprite);
	
	};
};

#endif