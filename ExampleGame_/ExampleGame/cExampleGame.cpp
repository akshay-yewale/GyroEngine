// Include Files
//==============

#include "cExampleGame.h"
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include "Engine\Graphics\sSpriteVertexFormat.h"
#include <stdint.h>
#include <array>
#include <Engine\Math\sVector.h>
#include "cCamera.h"
// Inherited Implementation
//=========================

// Run
//----


class GameObject
{
public:
	eae6320_001::Mesh * mesh = nullptr;
	
	eae6320_001::Mesh::Handle meshhandle;

	eae6320::Physics::sRigidBodyState i_rigidBodyComponent;

	GameObject() {
		
	};

	GameObject(eae6320::Math::sVector i_position, eae6320::Math::sVector i_velocity,
		eae6320::Math::sVector i_accelaration , eae6320::Math::cQuaternion i_orientation)
	{
		i_rigidBodyComponent = eae6320::Physics::sRigidBodyState();
		i_rigidBodyComponent.position = i_position;
		i_rigidBodyComponent.velocity = i_velocity;
		i_rigidBodyComponent.acceleration = i_accelaration;
		i_rigidBodyComponent.orientation = i_orientation;
	}

	void UpdateVelocity(const eae6320::Math::sVector i_velocity)
	{
		this->i_rigidBodyComponent.velocity = i_velocity;
	}

	void Update(const float i_secondsTointegrate) {
		this->i_rigidBodyComponent.Update(i_secondsTointegrate);
	}

	GameObject(eae6320::Math::sVector position, GameObject &gameObject, eae6320::Math::cQuaternion orientation) {
		this->i_rigidBodyComponent = eae6320::Physics::sRigidBodyState();
		this->i_rigidBodyComponent.position = position;
		i_rigidBodyComponent.orientation = orientation;// gameObject.i_rigidBodyComponent.orientation;
	}

	void Cleanup()
	{
	}
};


namespace {
	eae6320_001::Effect *effect = nullptr;
	eae6320_001::Sprites *sprite = nullptr;

	eae6320_001::Sprites * sprite_001 = nullptr;
	eae6320_001::Effect * effect_001 = nullptr;

	eae6320_001::Sprites * sprite_002 = nullptr;
	eae6320_001::Effect * effect_002 = nullptr;

	eae6320_001::Mesh * mesh_001 = nullptr;
	eae6320_001::Effect * effect_003 = nullptr;


	eae6320_001::Effect * effect_004 = nullptr;
	
	
	GameObject gameObject = GameObject(eae6320::Math::sVector(0, 0, -10.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject plane = GameObject(eae6320::Math::sVector(0, -0.2f, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject transCube = GameObject(eae6320::Math::sVector(0.0f, 0.0f, -20.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());
	// change position
	GameObject transSphere = GameObject(eae6320::Math::sVector(-5.0f, -0.2f, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());


	GameObject starShip = GameObject(eae6320::Math::sVector(10.0f, 0.0f, 0.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject redOrb_001 = GameObject(eae6320::Math::sVector(17.0f, 0.0f, -40.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject redOrb_002 = GameObject(eae6320::Math::sVector(3.0f, 0.0f, -80.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject redOrb_003 = GameObject(eae6320::Math::sVector(10.0f, 0.0f, -60.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject blueOrb_001 = GameObject(eae6320::Math::sVector(0.0f, 0.0f, -120.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	GameObject blueOrb_002 = GameObject(eae6320::Math::sVector(0.0f, 0.0f, -150.0f),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::cQuaternion());

	eae6320::Math::sVector velocity;

	eae6320::Graphics::cTexture::Handle someTexture1;
	eae6320::Graphics::cTexture::Handle someTexture2;
	eae6320::Graphics::cTexture::Handle RedOrb_001_Texture;
	eae6320::Graphics::cTexture::Handle RedOrb_002_Texture;
	eae6320::Graphics::cTexture::Handle BlueOrb_001_Texture;
	eae6320::Graphics::cTexture::Handle BlueOrb_002_Texture;
	eae6320::Graphics::cTexture::Handle LostState_Texture;


	eae6320::Graphics::cTexture::Handle starshipTexture;

	eae6320::Graphics::cTexture::Handle * _tempHandle_001;
	eae6320::Graphics::cTexture::Handle * RedOrbHandle;
	eae6320::Graphics::cTexture::Handle * BlueOrbHandle;

	float timePassed = 0.0f;

	//Camera objects
	eae6320_001::cCamera cCamera = eae6320_001::cCamera::cCamera(eae6320::Math::sVector(10.0f, 10.0f,30.0f), eae6320::Math::sVector(0, 0, 0),
		eae6320::Math::sVector(0, 0, 0), 0.785398f, 1.0f, 0.1f, 200.0f);

	std::vector<GameObject> Pickup;

	bool isGameEnded;

}

// Initialization / Clean Up
//--------------------------
eae6320::cResult eae6320::cExampleGame::Initialize()
{
		auto result = Results::Success;

		isGameEnded = false;

		if (!(result = eae6320_001::Effect::Initialize(effect_003))) {
		EAE6320_ASSERT(false);
		result = eae6320::Results::Failure;
		goto OnExit;
		}

		uint8_t renderstate = 0;
		eae6320::Graphics::RenderStates::EnableDepthBuffering(renderstate);

		// CHANGING RENDERSTATE TO 2
		if (!(result = effect_003->InitializeShadingData("MeshVertexShader_002", "MeshFragmentShader_001", renderstate))) {
		EAE6320_ASSERT(false);
		result = eae6320::Results::Failure;
		goto OnExit; 
		}

		// using for translucent mesh shaders
		if (!(result = eae6320_001::Effect::Initialize(effect_004))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		eae6320::Graphics::RenderStates::EnableAlphaTransparency(renderstate);
		eae6320::Graphics::RenderStates::EnableDepthBuffering(renderstate);

		if (!(result = effect_004->InitializeShadingData("MeshVertexShader_002", "MeshFragmentShader_002", renderstate))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Effect::Initialize(effect))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = effect->InitializeShadingData("VertexShader_001", "FragmentShader_001"))) {
			//EAE6320_ASSERT(false);
			EAE6320_ASSERTF(false, "Shaders not Found");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Effect::Initialize(effect_001))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = effect_001->InitializeShadingData("VertexShader_001", "FragmentShader_002"))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Effect::Initialize(effect_002))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = effect_002->InitializeShadingData("VertexShader_001", "FragmentShader_001"))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Sprites::Initialize(sprite))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = sprite->InitializeGeometry(eae6320_001::VertexFormat::sSpriteVertexFormat(-1.0f, 0.5f,0,0), 0.4f, 0.4f))) 	{	
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Sprites::Initialize(sprite_001))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = sprite_001->InitializeGeometry(eae6320_001::VertexFormat::sSpriteVertexFormat(-0.5f, -0.5f,0,0), 1.0f, 1.0f))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Sprites::Initialize(sprite_002))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = sprite_002->InitializeGeometry(eae6320_001::VertexFormat::sSpriteVertexFormat(1.5f, -1.0f , 0 ,0), 0.5f, 0.5f))) {
			EAE6320_ASSERT(false);
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/wolfrun_001.png",someTexture1))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/wolfrun_002.png", someTexture2))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/RedOrb_001.png", RedOrb_001_Texture))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}
	
		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/RedOrb_002.png", RedOrb_002_Texture))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}
		
		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/BlueOrb_001.png", BlueOrb_001_Texture))) {
				EAE6320_ASSERTF(false, "Initializing Effect failed");
				result = eae6320::Results::Failure;
				goto OnExit;
		}

		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/BlueOrb_002.png", BlueOrb_002_Texture))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/StarShip_Texture.png", starshipTexture))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}
		
		if (!(result = eae6320::Graphics::cTexture::s_manager.Load("data/Textures/LostState.png", LostState_Texture))) {
			EAE6320_ASSERTF(false, "Initializing Effect failed");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		{
			_tempHandle_001 = &someTexture1;
			RedOrbHandle = &RedOrb_001_Texture;
			BlueOrbHandle = &BlueOrb_001_Texture;
		}

		if (!(result =eae6320_001::Mesh::s_manager.Load("data/Mesh/Cube.lua", gameObject.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/Plane.lua", plane.meshhandle))) {
		EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
		result = eae6320::Results::Failure;
		goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/RedOrb.lua", transCube.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/BlueOrb.lua", transSphere.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/StarShip.lua", starShip.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/RedOrb.lua", redOrb_001.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/RedOrb.lua", redOrb_002.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/RedOrb.lua", redOrb_003.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/BlueOrb.lua", blueOrb_001.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		if (!(result = eae6320_001::Mesh::s_manager.Load("data/Mesh/BlueOrb.lua", blueOrb_002.meshhandle))) {
			EAE6320_ASSERTF(false, "Initializing Mesh handle Failed ");
			result = eae6320::Results::Failure;
			goto OnExit;
		}

		gameObject.i_rigidBodyComponent.angularVelocity_axis_local = eae6320::Math::sVector(0.0f, 1.0f, 0.0f);
		gameObject.i_rigidBodyComponent.angularSpeed = 3.0f;


OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	auto result = Results::Success;

	if (effect_003)
		effect_003->DecrementReferenceCount();

	if(sprite)
		sprite->DecrementReferenceCount();
	
	if(effect)
		effect->DecrementReferenceCount();

	if(sprite_001)
		sprite_001->DecrementReferenceCount();
	
	if(effect_001)
		effect_001->DecrementReferenceCount();

	if(sprite_002)
		sprite_002->DecrementReferenceCount();

	if(effect_002)
		effect_002->DecrementReferenceCount();

	if (effect_004)
		effect_004->DecrementReferenceCount();


	if (someTexture1)
		eae6320::Graphics::cTexture::s_manager.Release(someTexture1);

	if (someTexture2)
		eae6320::Graphics::cTexture::s_manager.Release(someTexture2);
		
	if (RedOrb_001_Texture)
		eae6320::Graphics::cTexture::s_manager.Release(RedOrb_001_Texture);

	if (RedOrb_002_Texture)
		eae6320::Graphics::cTexture::s_manager.Release(RedOrb_002_Texture);

	if (BlueOrb_001_Texture)
		eae6320::Graphics::cTexture::s_manager.Release(BlueOrb_001_Texture);

	if (BlueOrb_002_Texture)
		eae6320::Graphics::cTexture::s_manager.Release(BlueOrb_002_Texture);

	if (starshipTexture)
		eae6320::Graphics::cTexture::s_manager.Release(starshipTexture);


	if (LostState_Texture)
		eae6320::Graphics::cTexture::s_manager.Release(LostState_Texture);

		
	if(gameObject.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(gameObject.meshhandle);

	if(plane.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(plane.meshhandle);

	if (transCube.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(transCube.meshhandle);

	if (transSphere.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(transSphere.meshhandle);

	if (redOrb_001.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(redOrb_001.meshhandle);

	if (redOrb_002.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(redOrb_002.meshhandle);

	if (redOrb_003.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(redOrb_003.meshhandle);



	if (blueOrb_001.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(blueOrb_001.meshhandle);

	if (blueOrb_002.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(blueOrb_002.meshhandle);

	if (starShip.meshhandle)
		eae6320_001::Mesh::Mesh::s_manager.Release(starShip.meshhandle);


	return Results::Success;
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{

	eae6320::Graphics::SubmitClearDepthIndex(1.0f);

	eae6320::Graphics::SubmitClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	
	eae6320::Math::sVector _tempPosition = starShip.i_rigidBodyComponent.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate);

	eae6320::Math::cQuaternion _tempQuat = starShip.i_rigidBodyComponent.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate);
	
	GameObject temp = GameObject(_tempPosition, starShip, _tempQuat);
	
	//eae6320::Graphics::SubmitMeshData(effect_003, gameObject.mesh, &temp.i_rigidBodyComponent);
	//eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(gameObject.meshhandle), &temp.i_rigidBodyComponent);
	//eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(gameObject.meshhandle),
	//	&temp.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*_tempHandle_003));
	//eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(plane.meshhandle), 
	//	&plane.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*_tempHandle_003));

	
	eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(redOrb_001.meshhandle),
		&redOrb_001.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*RedOrbHandle));
	
	eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(redOrb_002.meshhandle),
		&redOrb_002.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*RedOrbHandle));

	eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(redOrb_003.meshhandle),
		&redOrb_003.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*RedOrbHandle));

	eae6320::Graphics::SubmitMeshData(effect_004, eae6320_001::Mesh::s_manager.Get(blueOrb_001.meshhandle),
		&blueOrb_001.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*BlueOrbHandle));

	eae6320::Graphics::SubmitMeshData(effect_004, eae6320_001::Mesh::s_manager.Get(blueOrb_002.meshhandle),
		&blueOrb_002.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(*BlueOrbHandle));


	eae6320::Graphics::SubmitMeshData(effect_003, eae6320_001::Mesh::s_manager.Get(starShip.meshhandle),
		&temp.i_rigidBodyComponent, eae6320::Graphics::cTexture::s_manager.Get(starshipTexture));

	
	eae6320::Graphics::SubmitEffectSpriteTexture(effect, sprite, eae6320::Graphics::cTexture::s_manager.Get(*RedOrbHandle));

	if(isGameEnded)
	eae6320::Graphics::SubmitEffectSpriteTexture(effect, sprite_001, eae6320::Graphics::cTexture::s_manager.Get(LostState_Texture));

	eae6320::Math::sVector _tempVec = cCamera.m_RigidbodyComponent.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate);

	eae6320::Math::cMatrix_transformation _tempTransform = cCamera.PredictFutureWorldToCameraTransfrom(_tempVec);

	eae6320_001::cCamera temp_Camera = eae6320_001::cCamera::cCamera(_tempVec, _tempTransform ,cCamera); //eae6320_001:cCamera(_tempVec, cCamera);

	eae6320::Graphics::SubmitCamera(temp_Camera);
}

void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	
	{
		if (redOrb_001.i_rigidBodyComponent.IsCollisionTriggered(starShip.i_rigidBodyComponent.position,6.0f))
		{
			isGameEnded = true;
		}

		if (redOrb_002.i_rigidBodyComponent.IsCollisionTriggered(starShip.i_rigidBodyComponent.position, 6.0f))
		{
			isGameEnded = true;
		}

		if (redOrb_003.i_rigidBodyComponent.IsCollisionTriggered(starShip.i_rigidBodyComponent.position, 6.0f))
		{
			isGameEnded = true;
		}


		if (blueOrb_001.i_rigidBodyComponent.IsCollisionTriggered(starShip.i_rigidBodyComponent.position, 5.0f))
		{
			blueOrb_001.i_rigidBodyComponent.position = eae6320::Math::sVector((float)(rand() % 20), 0.0f, starShip.i_rigidBodyComponent.position.z - 30.0f);
		}

		if (blueOrb_002.i_rigidBodyComponent.IsCollisionTriggered(starShip.i_rigidBodyComponent.position, 5.0f))
		{
			blueOrb_002.i_rigidBodyComponent.position = eae6320::Math::sVector((float)(rand() % 20), 0.0f, starShip.i_rigidBodyComponent.position.z - 67.0f);
		}
	}
	
	{
		if (redOrb_001.i_rigidBodyComponent.position.z > (starShip.i_rigidBodyComponent.position.z + 30.0f))
		{
			redOrb_001.i_rigidBodyComponent.position = eae6320::Math::sVector((float)(rand() % 20), 0.0f, starShip.i_rigidBodyComponent.position.z - 19.0f);
		}

		if (redOrb_002.i_rigidBodyComponent.position.z > (starShip.i_rigidBodyComponent.position.z + 30.0f))
		{
			redOrb_002.i_rigidBodyComponent.position = eae6320::Math::sVector((float)(rand() % 20), 0.0f, starShip.i_rigidBodyComponent.position.z - 31.0f);
		}

		if (redOrb_003.i_rigidBodyComponent.position.z > (starShip.i_rigidBodyComponent.position.z + 30.0f))
		{
			redOrb_003.i_rigidBodyComponent.position = eae6320::Math::sVector((float)(rand() % 20), 0.0f, starShip.i_rigidBodyComponent.position.z - 47.0f);
		}


	}

	starShip.Update(i_elapsedSecondCount_sinceLastUpdate);
	cCamera.UpdateBasedOnSimulationTime(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cExampleGame::UpdateSimulationBasedOnInput() {

	eae6320::Math::sVector tempVelocity;

	if(UserInput::IsKeyPressed('A'))
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(-1.0f, 0.0f, 0.0f));
		tempVelocity.x = -10.0f;
	}
	else if (UserInput::IsKeyPressed('D'))
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(1.0f, 0.0f, 0.0f));
		tempVelocity.x = 10.0f;
	}
	else
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
		tempVelocity.x = 0.0f;
	}

	if (UserInput::IsKeyPressed('S'))
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(-1.0f, 0.0f, 0.0f));
		tempVelocity.z = 10.0f;
	}
	else if (UserInput::IsKeyPressed('W'))
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(1.0f, 0.0f, 0.0f));
		tempVelocity.z = -10.0f;
	}
	else
	{
		//gameObject.UpdateVelocity(eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
		tempVelocity.y = 0.0f;
	}
	starShip.UpdateVelocity(tempVelocity);
	cCamera.UpdateVelocity(tempVelocity);




	/*
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		//_tempHandle_001 = &someTexture1;
		velocity.x = -1.00f;
		cCamera.UpdateVelocity(velocity);
		//gameObject.UpdateBasedOnVelocity(velocity, i_elapsedSecondCount_sinceLastUpdate);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		//_tempHandle_001 = &someTexture1;
		velocity.x = 1.00f;
		cCamera.UpdateVelocity(velocity);
	}
	else
	{
		velocity.x = 0.0f;
		cCamera.UpdateVelocity(velocity);
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		//_tempHandle_001 = &someTexture1;
		velocity.z = -1.00f;
		cCamera.UpdateVelocity(velocity);
		//gameObject.UpdateBasedOnVelocity(velocity, i_elapsedSecondCount_sinceLastUpdate);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		//_tempHandle_001 = &someTexture1;
		velocity.z = 1.00f;
		cCamera.UpdateVelocity(velocity);
		//gameObject.UpdateBasedOnVelocity(velocity, i_elapsedSecondCount_sinceLastUpdate);
	}
	else
	{
		velocity.y = 0.0f;
		velocity.z = 0.0f;
		cCamera.UpdateVelocity(velocity);
	}
	*/
}

void eae6320::cExampleGame::UpdateBasedOnTime(const float i_time) {
	timePassed += i_time;

	if (timePassed < 1.0f) {
		RedOrbHandle = &RedOrb_001_Texture;
		BlueOrbHandle = &BlueOrb_001_Texture;
	}
	else if (timePassed < 2.0f)
	{
		RedOrbHandle = &RedOrb_002_Texture;
		BlueOrbHandle = &BlueOrb_002_Texture;
	}
	else
	{
		timePassed = 0.0f;
	}
}

void eae6320::cExampleGame::UpdateBasedOnInput() {
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}
}

