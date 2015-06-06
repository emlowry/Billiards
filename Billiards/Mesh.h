#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Mesh
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureUV;

		Vertex(const glm::vec3& a_position = glm::vec3(0),
			   const glm::vec3& a_normal = glm::vec3(0),
			   const glm::vec2& a_textureUV = glm::vec2(0))
			: position(a_position),
			  normal(glm::normalize(a_normal)),
			  textureUV(a_textureUV) {}
	};

	unsigned int vertexArrayID;
	unsigned int vertexBufferID;
	unsigned int indexBufferID;

	Mesh(Vertex* a_vertices, unsigned int a_vertexCount);
	Mesh(Vertex* a_vertices, unsigned int a_vertexCount,
		 unsigned int* a_indices, unsigned int a_indexCount);

	static Mesh GenerateCubeMesh();
	static Mesh GenerateGridMesh(float a_tileSize = 1, unsigned int a_tiles = 20);
	static Mesh GenerateSphereMesh(unsigned int a_rows = 8) { return GenerateSphereMesh(a_rows, a_rows * 2); }
	static Mesh GenerateSphereMesh(unsigned int a_rows, unsigned int a_columns);
};