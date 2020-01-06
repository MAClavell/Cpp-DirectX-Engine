#pragma once
#include <DirectXMath.h>
#include "GameObject.h"

// --------------------------------------------------------
// A camera definition.
// --------------------------------------------------------
class Camera : public Component
{
private:
	//Matrices
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 rawView;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 rawProjection;

	//Transformation data
	DirectX::XMFLOAT3 up;

public:
	// --------------------------------------------------------
	// Constructor - Set up the camera
	// (Remember to create the projection matrix right after!)
	// --------------------------------------------------------
	Camera(GameObject* gameObject);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Camera();

	// --------------------------------------------------------
	// Create the camera's view matrix from fields
	// --------------------------------------------------------
	void CreateViewMatrix();

	// --------------------------------------------------------
	// Get the camera's view matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetViewMatrix();

	// --------------------------------------------------------
	// Get the camera's raw (untransposed) view matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetRawViewMatrix();

	// --------------------------------------------------------
	// Create the camera's projection matrix from parameters
	//
	// fov - FOV of the camera
	// aspectRatio - Aspect ratio of the camera (width / height)
	// nearClip - Near clip plane distance
	// farClip - Far clip plane distance
	// --------------------------------------------------------
	void CreateProjectionMatrix(float fov, float aspectRatio, 
									float nearClip, float farClip);

	// --------------------------------------------------------
	// Get the camera's projection matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	// --------------------------------------------------------
	// Get the raw (untransposed) camera's projection matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetRawProjectionMatrix();
};

