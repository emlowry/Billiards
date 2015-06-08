#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Mesh.h"
#include "Texture.h"
#include "Engine.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

namespace Renderer
{
	// struct describing a light source
	struct Light
	{
		glm::vec3 color = glm::vec3(1, 1, 1);
		glm::vec3 direction = glm::vec3(0, 0, 0);	// zero vector = point light
		glm::vec3 position = glm::vec3(0, 0, 0);

		// intensity = power / ((distance)^(2 * attenuation))
		float power = 1;		// usually 1 if there's no attenuation
		float attenuation = 0;	// 0 means no attenuation

		// only used for spot lights:
		float angle = 0;	// angle between axis and edge of spot light cone, 0 = directional light
		float blur = 0;		// 0 = sharp cutoff, 1 = radial gradient
	};

	glm::vec3 GetCameraPosition();
	void SetCameraPosition(const glm::vec3& a_position);

	glm::vec3 GetAmbientLight();
	void SetAmbientLight(const glm::vec3& a_light);

	std::vector<Light> GetLights();
	void SetLights(const std::vector<Light>& a_lights);
	void AddLight(const Light& a_light);
	void ClearLights();

	bool LoadShader();
	bool ShaderIsLoaded();
	void DestroyShader();

	void LoadMesh(Mesh& a_mesh, Mesh::Vertex* a_vertices, unsigned int a_vertexCount,
				  unsigned int* a_indices, unsigned int a_indexCount);
	void DrawMesh(const Mesh& a_mesh, const Texture& a_texture = Texture(),
				  const glm::mat4& a_modelMatrix = Engine::IDENTITY_MATRIX);

	void QueueMesh(const Mesh& a_mesh, const Texture& a_texture = Texture(),
				   const glm::mat4& a_modelMatrix = Engine::IDENTITY_MATRIX);
	void DrawQueuedMeshes();
	void ClearMeshQueue();
}

#endif // _RENDERER_H_
