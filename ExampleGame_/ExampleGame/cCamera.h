#ifndef  CAMERA_H
#define CAMERA_H

#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Physics/sRigidBodyState.h>

namespace eae6320_001{
	class cCamera
	{
	private:

	public:
		eae6320::Math::cMatrix_transformation m_LocalCameraToWorld;
		eae6320::Math::cMatrix_transformation m_WorldToCameraTransformation;
		eae6320::Physics::sRigidBodyState m_RigidbodyComponent;

		cCamera();

		cCamera(eae6320::Math::sVector position, cCamera &cameraObject);

		cCamera(eae6320::Math::sVector position, eae6320::Math::cMatrix_transformation worldToCameraTransform, cCamera &cameraObject);

		cCamera(eae6320::Math::sVector i_position, eae6320::Math::sVector i_velocity, eae6320::Math::sVector i_accelaration,
			const float i_verticalFieldOfView_inRadians,
			const float i_aspectRatio,
			const float i_z_nearPlane, const float i_z_farPlane);

		~cCamera();

		void UpdateVelocity(eae6320::Math::sVector i_vector);
		void UpdateAccelaration(eae6320::Math::sVector i_vector);
		void UpdatePosition(eae6320::Math::sVector i_vector);
		void UpdateRigidbodyComponent(eae6320::Math::sVector i_vector1, eae6320::Math::sVector i_vector2,eae6320::Math::sVector i_vector3);
	
		eae6320::Math::sVector PredictFuturePosition(const float i_timeToInterpolate);
		eae6320::Math::cMatrix_transformation PredictFutureWorldToCameraTransfrom(eae6320::Math::sVector position);

		void UpdateBasedOnSimulationTime(const float i_simulationTime);
	};
};
#endif
