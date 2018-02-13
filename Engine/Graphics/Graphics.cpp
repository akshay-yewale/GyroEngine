// Include Files
//==============


#include "Graphics.h"
#include "cConstantBuffer.h"
#include "cSamplerState.h"
#include "ConstantBufferFormats.h"
#include "sContext.h"
#include "GraphicsAbstractionLayer.h"

#include <Engine/Concurrency/cEvent.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <utility>
#include <Engine\UserOutput\UserOutput.h>


#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>




namespace {

	// For depth 
	float m_depthClearindex;

	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame(eae6320::Graphics::ConstantBufferTypes::PerFrame);
	// In our class we will only have a single sampler state
	eae6320::Graphics::cSamplerState s_samplerState;
	//Constant Draw Call
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perDrawCall(eae6320::Graphics::ConstantBufferTypes::PerDrawCall);

	struct sEffectMeshTextureDrawCallData {
		eae6320_001::Effect* i_effect;
		eae6320_001::Mesh * i_mesh;
		eae6320::Graphics::cTexture * i_texture;
		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall i_perDrawCall;

//		sEffectMeshTextureDrawCallData tempStruct;
	};
	
	// Submission Data 
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		
		eae6320::Graphics::ConstantBufferFormats::sPerClearColor constantData_perClearColor;
		
		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall constantData_perDrawCall;

		std::vector<std::pair<eae6320_001::Effect*, eae6320_001::Sprites*>> constData_perEffectSprite;
		
		std::vector< std::tuple < eae6320_001::Effect*, eae6320_001::Sprites*, eae6320::Graphics::cTexture *>> constData_EffectSpriteTexture;
		
		std::vector<std::pair<eae6320_001::Effect*, eae6320_001::Mesh *>>  constData_MeshEffect;
		
		std::vector<std::tuple<eae6320_001::Effect*, eae6320_001::Mesh *,
			eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>> constData_EffectMeshPosition;

		std::vector<sEffectMeshTextureDrawCallData> constData_EffectMeshPositionTexture;

		std::vector<sEffectMeshTextureDrawCallData> constData_EffectTransMeshPositionTexture;
		
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be getting populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated, 
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

	//Abstraction Class for Graphics class to handle platform specific code
	eae6320_001::cGraphicsAbstractionLayer cGraphicsAbstractionLayer;
}

//namespace {
//	eae6320::cResult InitializeGeometry();
//	eae6320::cResult InitializeShadingData();
//	//======================================//
//	eae6320::cResult InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);
//	//======================================//
//}


void eae6320::Graphics::SubmitEffectSpritePair(eae6320_001::Effect* i_effect, eae6320_001::Sprites* i_sprite) {
	auto& constEffectSpriteBind = s_dataBeingSubmittedByApplicationThread->constData_perEffectSprite;
	i_effect->IncrementReferenceCount();
	i_sprite->IncrementReferenceCount();
	constEffectSpriteBind.push_back(std::make_pair(i_effect, i_sprite));
}

void eae6320::Graphics::SubmitEffectSpriteTexture(eae6320_001::Effect* i_effect, eae6320_001::Sprites* i_sprite, eae6320::Graphics::cTexture * i_textureHandle) {

	auto& _temp = s_dataBeingSubmittedByApplicationThread->constData_EffectSpriteTexture;
	i_effect->IncrementReferenceCount();
	i_sprite->IncrementReferenceCount();
	//eae6320::Graphics::cTexture::s_manager.Get(i_textureHandle)->IncrementReferenceCount();
	i_textureHandle->IncrementReferenceCount();
	_temp.push_back(std::make_tuple(i_effect, i_sprite, i_textureHandle));
	
}

void eae6320::Graphics::SubmitMeshData(eae6320_001::Effect * i_effect, eae6320_001::Mesh* i_mesh, eae6320::Physics::sRigidBodyState * i_rigidBodyComponent) {
	
	auto& _temp = s_dataBeingSubmittedByApplicationThread->constData_EffectMeshPosition;
	i_effect->IncrementReferenceCount();
	i_mesh->IncrementReferenceCount();
	eae6320::Graphics::ConstantBufferFormats::sPerDrawCall temp_perDrawCall;
	temp_perDrawCall.s_transform_localToWorld = eae6320::Math::cMatrix_transformation::cMatrix_transformation(
		i_rigidBodyComponent->orientation , i_rigidBodyComponent->position);
	_temp.push_back(std::make_tuple(i_effect, i_mesh, temp_perDrawCall));
}


void eae6320::Graphics::SubmitMeshData(eae6320_001::Effect * i_effect, eae6320_001::Mesh* i_mesh, 
	eae6320::Physics::sRigidBodyState * i_rigidBodyComponent, eae6320::Graphics::cTexture * i_textureHandle) {
	
	if (i_effect->s_renderState.IsDepthBufferingEnabled() && i_effect->s_renderState.IsAlphaTransparencyEnabled())
	{
		auto& _temp = s_dataBeingSubmittedByApplicationThread->constData_EffectTransMeshPositionTexture;
		i_effect->IncrementReferenceCount();
		i_mesh->IncrementReferenceCount();
		i_textureHandle->IncrementReferenceCount();

		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall temp_perDrawCall;

		temp_perDrawCall.s_transform_localToWorld = eae6320::Math::cMatrix_transformation::cMatrix_transformation(
			i_rigidBodyComponent->orientation, i_rigidBodyComponent->position);

		sEffectMeshTextureDrawCallData tempStruct;
		tempStruct.i_effect = i_effect;
		tempStruct.i_mesh = i_mesh;
		tempStruct.i_texture = i_textureHandle;
		tempStruct.i_perDrawCall = temp_perDrawCall;
		_temp.push_back(tempStruct);
	}
	

	else if (i_effect->s_renderState.IsDepthBufferingEnabled())
	{
		auto& _temp = s_dataBeingSubmittedByApplicationThread->constData_EffectMeshPositionTexture;
		i_effect->IncrementReferenceCount();
		i_mesh->IncrementReferenceCount();
		i_textureHandle->IncrementReferenceCount();

		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall temp_perDrawCall;

		temp_perDrawCall.s_transform_localToWorld = eae6320::Math::cMatrix_transformation::cMatrix_transformation(
			i_rigidBodyComponent->orientation, i_rigidBodyComponent->position);

		sEffectMeshTextureDrawCallData tempStruct;
		tempStruct.i_effect = i_effect;
		tempStruct.i_mesh = i_mesh;
		tempStruct.i_texture = i_textureHandle;
		tempStruct.i_perDrawCall = temp_perDrawCall;
		_temp.push_back(tempStruct);
	}
}


void eae6320::Graphics::SubmitClearDepthIndex(float i_index)
{
	m_depthClearindex = i_index;
}

void eae6320::Graphics::SubmitClearColor(float i_r, float i_g, float i_b, float i_a) {
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantClearColor_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perClearColor;
	constantClearColor_perFrame.r = i_r;
	constantClearColor_perFrame.g = i_g;
	constantClearColor_perFrame.b = i_b;
	constantClearColor_perFrame.a = i_a;
}

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitCamera(eae6320_001::cCamera cCamera) {
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_transform_worldToCamera = cCamera.m_WorldToCameraTransformation;
	constantData_perFrame.g_transform_cameraToProjected = cCamera.m_LocalCameraToWorld;
}


eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

//Initialize Function
eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// Initialize the asset managers
	{
		if (!(result = cShader::s_manager.Initialize()))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_perFrame.Initialize())
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perFrame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (result = s_constantBuffer_perDrawCall.Initialize())
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perDrawCall.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (result = s_samplerState.Initialize())
		{
			// There is only a single sampler state that is re-used
			// and so it can be bound at initialization time and never unbound
			s_samplerState.Bind();
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}
	// Initialize the views
	{
		if (!(result = cGraphicsAbstractionLayer.InitializeViews(i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = eae6320::Results::Success;
	cGraphicsAbstractionLayer.CleanUp();

	{
		auto& _effectSpriteTextureData = s_dataBeingRenderedByRenderThread->constData_EffectSpriteTexture;
		std::vector<std::tuple<eae6320_001::Effect *, eae6320_001::Sprites*, eae6320::Graphics::cTexture *>>::iterator iter_EffectSpriteTexture;
		for (iter_EffectSpriteTexture = _effectSpriteTextureData.begin();
			iter_EffectSpriteTexture != _effectSpriteTextureData.end();
			iter_EffectSpriteTexture++) {
			std::get<0>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			//eae6320::Graphics::cTexture::s_manager.Release(std::get<2>(*iter_EffectSpriteTexture));
			std::get<2>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			std::get<1>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
		}
	}
	{
		auto& _effectSpriteTextureData = s_dataBeingSubmittedByApplicationThread->constData_EffectSpriteTexture;
		std::vector<std::tuple<eae6320_001::Effect *, eae6320_001::Sprites*, eae6320::Graphics::cTexture *>>::iterator iter_EffectSpriteTexture;
		for (iter_EffectSpriteTexture = _effectSpriteTextureData.begin();
			iter_EffectSpriteTexture != _effectSpriteTextureData.end();
			iter_EffectSpriteTexture++) {
			std::get<0>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			std::get<2>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			std::get<1>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
		}
	}

	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}
	{
		auto& _effectmeshposition = s_dataBeingSubmittedByApplicationThread->constData_EffectMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}

	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectTransMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}
	{
		auto& _effectmeshposition = s_dataBeingSubmittedByApplicationThread->constData_EffectTransMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}


	//{
	//	auto& _effectMeshAddress = s_dataBeingSubmittedByApplicationThread->constData_MeshEffect;
	//	std::vector<std::pair<eae6320_001::Effect*, eae6320_001::Mesh*>>::iterator _iterator;
	//	for (_iterator = _effectMeshAddress.begin(); _iterator != _effectMeshAddress.end();
	//		_iterator++)
	//	{
	//		_iterator->first->DecrementReferenceCount();
	//		_iterator->second->DecrementReferenceCount();
	//	}
	//}
	//{
	//	auto& _effectMeshAddress = s_dataBeingRenderedByRenderThread->constData_MeshEffect;
	//	std::vector<std::pair<eae6320_001::Effect*, eae6320_001::Mesh*>>::iterator _iterator;
	//	for (_iterator = _effectMeshAddress.begin(); _iterator != _effectMeshAddress.end();
	//		_iterator++)
	//	{
	//		_iterator->first->DecrementReferenceCount();
	//		_iterator->second->DecrementReferenceCount();
	//	}
	//}
	//

	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPosition;
		std::vector<std::tuple<eae6320_001::Effect*, eae6320_001::Mesh *,
			eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>>::iterator _iter;
		{
			for (_iter = _effectmeshposition.begin();
				_iter != _effectmeshposition.end();
				_iter++)
			{

				std::get<0>(*_iter)->DecrementReferenceCount();
				std::get<1>(*_iter)->DecrementReferenceCount();

			}
		}
	}
	
	{
		auto& _effectmeshposition = s_dataBeingSubmittedByApplicationThread->constData_EffectMeshPosition;
		std::vector<std::tuple<eae6320_001::Effect*, eae6320_001::Mesh *,
			eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>>::iterator _iter;
		{
			for (_iter = _effectmeshposition.begin();
				_iter != _effectmeshposition.end();
				_iter++)
			{

				std::get<0>(*_iter)->DecrementReferenceCount();
				std::get<1>(*_iter)->DecrementReferenceCount();

			}
		}
	}

	{
		const auto localResult = s_constantBuffer_perFrame.CleanUp();
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
		const auto localResult = s_constantBuffer_perDrawCall.CleanUp();
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
		const auto localResult = s_samplerState.CleanUp();
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
		const auto localResult = cShader::s_manager.CleanUp();
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
		const auto localResult = sContext::g_context.CleanUp();
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

bool Myfunction(std::pair<sEffectMeshTextureDrawCallData, eae6320::Math::sVector> i, std::pair<sEffectMeshTextureDrawCallData, eae6320::Math::sVector> j)
{
	return i.second.z < j.second.z;
}

void eae6320::Graphics::RenderFrame() {



	const auto result = Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
	if (result)
	{
		
		// Switch the render data pointers so that
		// the data that the application just submitted becomes the data that will now be rendered
		std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);


		// Once the pointers have been swapped the application loop can submit new data
		const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
		if (!result)
		{
			EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
			Logging::OutputError("Failed to signal that new render data can be submitted");
			UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}
	else
	{
		EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
		Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
		UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
			" The application is probably in a bad state and should be exited");
		return;
	}
 

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	{
		cGraphicsAbstractionLayer.ClearDepthBuffer(m_depthClearindex);
		eae6320::Graphics::ConstantBufferFormats::sPerClearColor& constData_perClearColor = s_dataBeingRenderedByRenderThread->constantData_perClearColor;
		const float color[4] = { constData_perClearColor.r, constData_perClearColor.g, constData_perClearColor.b, constData_perClearColor.a };
		cGraphicsAbstractionLayer.SetClearColor(color);
	}


	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update(&constantData_perFrame);
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer")

	/*{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPosition;
		std::vector<std::tuple<eae6320_001::Effect*, eae6320_001::Mesh *,
			eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>>::iterator _iter;
		{
			for (_iter = _effectmeshposition.begin();
				_iter != _effectmeshposition.end();
				_iter++)
			{
				s_constantBuffer_perDrawCall.Update(&std::get<2>(*_iter));
				std::get<0>(*_iter)->BindShadingData();
				std::get<1>(*_iter)->Draw();

			}
		}
	}*/
	
	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
			{
				sEffectMeshTextureDrawCallData temp = *iter_effectMesh;
				s_constantBuffer_perDrawCall.Update(&temp.i_perDrawCall);
				temp.i_effect->BindShadingData();
				temp.i_texture->Bind(0);
				temp.i_mesh->Draw();
			}
	}

		std::vector<std::pair<sEffectMeshTextureDrawCallData, eae6320::Math::sVector>> data_meshEffect_transform;
	{
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectTransMeshPositionTexture;

		{

			std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
			for (iter_effectMesh = _effectmeshposition.begin();
				iter_effectMesh != _effectmeshposition.end();
				iter_effectMesh++)

			{
				eae6320::Math::sVector i_position = constantData_perFrame.g_transform_worldToCamera * iter_effectMesh->i_perDrawCall.s_transform_localToWorld.GetTranslation();
				data_meshEffect_transform.push_back(std::make_pair(*iter_effectMesh, i_position));
			}
		}

	
		std::sort(data_meshEffect_transform.begin(), data_meshEffect_transform.end(), Myfunction);
		{

			std::vector<std::pair<sEffectMeshTextureDrawCallData, eae6320::Math::sVector>>::iterator iter_effectMesh;

			for (iter_effectMesh = data_meshEffect_transform.begin();
				iter_effectMesh != data_meshEffect_transform.end();
				iter_effectMesh++)
			{
				sEffectMeshTextureDrawCallData temp = iter_effectMesh->first;

				s_constantBuffer_perDrawCall.Update(&temp.i_perDrawCall);
				temp.i_effect->BindShadingData();
				temp.i_texture->Bind(0);
				temp.i_mesh->Draw();
			}
		}
	}

	/*{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			sEffectMeshTextureDrawCallData temp = *iter_effectMesh;
			s_constantBuffer_perDrawCall.Update(&temp.i_perDrawCall);
			temp.i_effect->BindShadingData();
			temp.i_texture->Bind(0);
			temp.i_mesh->Draw();
		}
	}
*/
	{
		auto& _effectSpriteTextureData = s_dataBeingRenderedByRenderThread->constData_EffectSpriteTexture;
		std::vector<std::tuple<eae6320_001::Effect *, eae6320_001::Sprites*, eae6320::Graphics::cTexture *>>::iterator iter_EffectSpriteTexture;
		for (iter_EffectSpriteTexture = _effectSpriteTextureData.begin();
			iter_EffectSpriteTexture != _effectSpriteTextureData.end();
			iter_EffectSpriteTexture++) {
			std::get<0>(*iter_EffectSpriteTexture)->BindShadingData();
			std::get<2>(*iter_EffectSpriteTexture)->Bind(0);
			std::get<1>(*iter_EffectSpriteTexture)->Draw();
		}
	}
	

	cGraphicsAbstractionLayer.SwapDisplayBuffers();



	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}

	{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectTransMeshPositionTexture;
		std::vector<sEffectMeshTextureDrawCallData>::iterator iter_effectMesh;
		for (iter_effectMesh = _effectmeshposition.begin();
			iter_effectMesh != _effectmeshposition.end();
			iter_effectMesh++)
		{
			iter_effectMesh->i_effect->DecrementReferenceCount();
			iter_effectMesh->i_texture->DecrementReferenceCount();
			iter_effectMesh->i_mesh->DecrementReferenceCount();

		}
		_effectmeshposition.clear();
	}



	/*{
		auto& _effectmeshposition = s_dataBeingRenderedByRenderThread->constData_EffectMeshPosition;
		std::vector<std::tuple<eae6320_001::Effect*, eae6320_001::Mesh *,
			eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>>::iterator _iter;
		{
			for (_iter = _effectmeshposition.begin();
				_iter != _effectmeshposition.end();
				_iter++)
			{
				std::get<0>(*_iter)->DecrementReferenceCount();
				std::get<1>(*_iter)->DecrementReferenceCount();
			}
		}
		_effectmeshposition.clear();
	}*/

	{
		auto& _effectSpriteTextureData = s_dataBeingRenderedByRenderThread->constData_EffectSpriteTexture;
		std::vector<std::tuple<eae6320_001::Effect *, eae6320_001::Sprites*, eae6320::Graphics::cTexture *>>::iterator iter_EffectSpriteTexture;
		for (iter_EffectSpriteTexture = _effectSpriteTextureData.begin();
			iter_EffectSpriteTexture != _effectSpriteTextureData.end();
			iter_EffectSpriteTexture++) {
			std::get<0>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			std::get<2>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
			std::get<1>(*iter_EffectSpriteTexture)->DecrementReferenceCount();
		}
		_effectSpriteTextureData.clear();
	}
}

