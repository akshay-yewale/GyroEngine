#include "cCamera.h"



eae6320_001::cCamera::cCamera()
{
}

eae6320_001::cCamera::cCamera(eae6320::Math::sVector i_position, eae6320::Math::sVector i_velocity, eae6320::Math::sVector i_accelaration,
	const float i_verticalFieldOfView_inRadians, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane)
{
	this->m_RigidbodyComponent.position = i_position;
	this->m_RigidbodyComponent.acceleration = i_accelaration;
	this->m_RigidbodyComponent.velocity = i_velocity;

	this->m_LocalCameraToWorld = eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(i_verticalFieldOfView_inRadians, i_aspectRatio, i_z_nearPlane, i_z_farPlane);
	this->m_WorldToCameraTransformation = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(this->m_RigidbodyComponent.orientation, this->m_RigidbodyComponent.position);


}

eae6320_001::cCamera::~cCamera()
{
}

void eae6320_001::cCamera::UpdateVelocity(eae6320::Math::sVector i_vector){
	this->m_RigidbodyComponent.velocity = i_vector;
}

void eae6320_001::cCamera::UpdateAccelaration(eae6320::Math::sVector i_vector) {
	this->m_RigidbodyComponent.acceleration = i_vector;
}

void eae6320_001::cCamera::UpdatePosition(eae6320::Math::sVector i_vector) {
	this->m_RigidbodyComponent.position = i_vector;
}

void eae6320_001::cCamera::UpdateRigidbodyComponent(eae6320::Math::sVector i_position, eae6320::Math::sVector i_accelaration, eae6320::Math::sVector i_velocity) {
	this->m_RigidbodyComponent.velocity = i_velocity;
	this->m_RigidbodyComponent.acceleration = i_accelaration;
	this->m_RigidbodyComponent.position = i_position;
}

void eae6320_001::cCamera::UpdateBasedOnSimulationTime(const float i_simulationTime) {
	//const float t = i_simulationTime;
	this->m_WorldToCameraTransformation = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(this->m_RigidbodyComponent.orientation, this->m_RigidbodyComponent.position);
	this->m_RigidbodyComponent.Update(i_simulationTime);
}

eae6320_001::cCamera::cCamera(eae6320::Math::sVector position, eae6320_001::cCamera &cCamera) {
	this->m_LocalCameraToWorld = cCamera.m_LocalCameraToWorld;
	this->m_WorldToCameraTransformation = cCamera.m_WorldToCameraTransformation;
	this->m_RigidbodyComponent.position = position;
}


eae6320_001::cCamera::cCamera(eae6320::Math::sVector position, eae6320::Math::cMatrix_transformation worldToCameraTransform, eae6320_001::cCamera &cCamera) {
	this->m_LocalCameraToWorld = cCamera.m_LocalCameraToWorld;
	this->m_WorldToCameraTransformation = worldToCameraTransform;
	this->m_RigidbodyComponent.position = position;
}


eae6320::Math::sVector eae6320_001::cCamera::PredictFuturePosition(const float i_time)
{
	return this->m_RigidbodyComponent.position + (this->m_RigidbodyComponent.velocity * i_time);
}

eae6320::Math::cMatrix_transformation eae6320_001::cCamera::PredictFutureWorldToCameraTransfrom(eae6320::Math::sVector position) {
	return  eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(this->m_RigidbodyComponent.orientation, position);
}
