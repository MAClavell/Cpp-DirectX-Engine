#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

// --------------------------------------------------------
// A mesh renderer definition.
//
// Allows a mesh to be rendered
// --------------------------------------------------------
class MeshRenderer : public Component
{
private:
	//Rendering
	Mesh* mesh;
	Material* material;
	std::string identifier;

public:
	// --------------------------------------------------------
	// Constructor - Set up the MeshRenderer.
	// MeshRenderers are automatically added to the Renderer
	//
	// mesh	- The mesh that this entity displays
	// material - The material this entity uses.
	// --------------------------------------------------------
	MeshRenderer(GameObject* gameObject, Mesh* mesh, Material* material);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~MeshRenderer();

	// --------------------------------------------------------
	// Get the material this entity uses
	// --------------------------------------------------------
	Material* GetMaterial();
	
	// --------------------------------------------------------
	// Get the mesh this entity uses
	// --------------------------------------------------------
	Mesh* GetMesh();

	// --------------------------------------------------------
	// Get the material/mesh identifier
	// --------------------------------------------------------
	std::string GetMatMeshIdentifier();
};