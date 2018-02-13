/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include <vector>
#include <tuple>
#include "Effect.h"
#include "Sprites.h"
#include "sColor.h"
#include "cTexture.h"
#include "Mesh.h"

#include <Engine\Math\sVector.h>
#include <ExampleGame_\ExampleGame\cCamera.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif



// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		// Submission
		//-----------
		
		void SubmitClearDepthIndex(float depthClearindex);

		void SubmitClearColor(float r, float g, float b, float a);

		void SubmitEffectSpritePair(eae6320_001::Effect*, eae6320_001::Sprites*);

		void SubmitEffectSpriteTexture(eae6320_001::Effect* i_effect, eae6320_001::Sprites* i_sprite, 
			eae6320::Graphics::cTexture * i_textureHandle);

		

		void SubmitMeshData(eae6320_001::Effect * i_effect, eae6320_001::Mesh* i_mesh, eae6320::Physics::sRigidBodyState * i_rigidBodyComponent);

		void SubmitCamera(eae6320_001::cCamera cCamera);

		void SubmitMeshData(eae6320_001::Effect * i_effect, eae6320_001::Mesh* i_mesh, eae6320::Physics::sRigidBodyState * i_rigidBodyComponent, eae6320::Graphics::cTexture * i_textureHandle);
		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );

		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
			uint16_t resolutionWidth, resolutionHeight;
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
	#elif defined( EAE6320_PLATFORM_GL )
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();
	}
}

#endif	// EAE6320_GRAPHICS_H
