/*
	This file defines the layout of the constant data
	that the CPU sends to the GPU

	These must exactly match the constant buffer definitions in shader programs.
*/

#ifndef EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
#define EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H

// Include Files
//==============

#include "Configuration.h"
#include <Engine/Math/cMatrix_transformation.h>
// Format Definitions
//===================

namespace eae6320
{
	namespace Graphics
	{
		namespace ConstantBufferFormats
		{
			struct sPerFrame
			{
				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToProjected;

				float g_elapsedSecondCount_systemTime = 0.0f;
				float g_elapsedSecondCount_simulationTime = 0.0f;
				float padding[2];	// For float4 alignment
			};

			struct sPerMaterial
			{
				struct
				{
					float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
				} g_color;
			};

			struct sPerClearColor
			{
				float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
			};

			struct sPerDrawCall
			{
				/*struct {
					float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
				}g_position;*/
				Math::cMatrix_transformation s_transform_localToWorld;

				/*sPerDrawCall(const eae6320::Math::cQuaternion& i_rotation, const eae6320::Math::sVector& i_translation) {
					s_transform_localToWorld = eae6320::Math::cMatrix_transformation(i_rotation, i_translation);
				}*/
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
