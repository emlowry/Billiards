#include "Mesh.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

// load vertex and index data into buffers stored in the given mesh struct
static void Load(Mesh& a_mesh, Mesh::Vertex* a_vertices, unsigned int a_vertexCount,
				 unsigned int* a_indices, unsigned int a_indexCount)
{
	// create and bind vertex array
	glGenVertexArrays(1, &a_mesh.vertexArrayID);
	glBindVertexArray(a_mesh.vertexArrayID);

	// create buffers
	glGenBuffers(1, &a_mesh.vertexBufferID);
	glGenBuffers(1, &a_mesh.indexBufferID);

	// bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, a_mesh.vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a_mesh.indexBufferID);

	// load data
	glBufferData(GL_ARRAY_BUFFER, a_vertexCount * sizeof(Mesh::Vertex), a_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);

	// enable attribute locations
	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// normal
	glEnableVertexAttribArray(2);	// textureUV

	// describe attribute locations
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Mesh::Vertex), ((char*)0) + sizeof(glm::vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), ((char*)0) + sizeof(glm::vec3) * 2);

	// unbind vertex array and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// create a mesh from vertex data
Mesh::Mesh(Vertex* a_vertices, unsigned int a_vertexCount)
{
	unsigned int* indices = new unsigned int[a_vertexCount];
	for (unsigned int i = 0; i < a_vertexCount; ++i)
		indices[i] = i;
	Load(*this, a_vertices, a_vertexCount, indices, a_vertexCount);
	delete[] indices;
}

// create a mesh from vertex and index data
Mesh::Mesh(Vertex* a_vertices, unsigned int a_vertexCount,
		   unsigned int* a_indices, unsigned int a_indexCount)
{
	Load(*this, a_vertices, a_vertexCount, a_indices, a_indexCount);
}

Mesh Mesh::GenerateCubeMesh()
{
	const unsigned int vertexCount = 24;
	Vertex vertices[vertexCount] =
	{
		Vertex(glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0)),
		Vertex(glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0)),
		Vertex(glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),
		Vertex(glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1)),

		Vertex(glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 0)),
		Vertex(glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 0)),
		Vertex(glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 1)),
		Vertex(glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 1)),

		Vertex(glm::vec3(-1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0)),
		Vertex(glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(1, 0)),
		Vertex(glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
		Vertex(glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0, 1)),

		Vertex(glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
		Vertex(glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
		Vertex(glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(1, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),

		Vertex(glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
		Vertex(glm::vec3(1, -1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 0)),
		Vertex(glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec3(1, 0, 0), glm::vec2(0, 1)),

		Vertex(glm::vec3(-1, -1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 0)),
		Vertex(glm::vec3(-1, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 0)),
		Vertex(glm::vec3(-1, 1, -1), glm::vec3(-1, 0, 0), glm::vec2(1, 1)),
		Vertex(glm::vec3(-1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 1))
	};

	const unsigned int indexCount = 36;
	unsigned int indices[indexCount] = { 0, 1, 2, 2, 3, 0,
										 4, 5, 6, 6, 7, 4,
										 8, 9, 10, 10, 11, 8,
										 12, 13, 14, 14, 15, 12,
										 16, 17, 18, 18, 19, 16,
										 20, 21, 22, 22, 23, 20 };

	return Mesh(vertices, vertexCount, indices, indexCount);
}

Mesh Mesh::GenerateGridMesh(float a_tileSize, unsigned int a_tiles)
{
	const unsigned int vertexCount = (a_tiles + 1) * (a_tiles + 1);
	Vertex* vertices = new Vertex[vertexCount];
	float halfSize = a_tileSize * a_tiles / 2;
	for (unsigned int i = 0; i < a_tiles + 1; ++i)
	{
		for (unsigned int j = 0; j < a_tiles + 1; ++j)
		{
			vertices[i * (a_tiles + 1) + j] =
				Vertex(glm::vec3(i * a_tileSize - halfSize, halfSize - j * a_tileSize, 0),
				glm::vec3(0, 0, 1), glm::vec2(i, j));
		}
	}

	const unsigned int indexCount = a_tiles * a_tiles * 6;
	unsigned int* indices = new unsigned int[indexCount];
	for (unsigned int i = 0; i < a_tiles; ++i)
	{
		for (unsigned int j = 0; j < a_tiles; ++j)
		{
			indices[6 * (i * a_tiles + j)] = i * (a_tiles + 1) + j;
			indices[6 * (i * a_tiles + j) + 1] = i * (a_tiles + 1) + j + 1;
			indices[6 * (i * a_tiles + j) + 2] = (i + 1) * (a_tiles + 1) + j + 1;
			indices[6 * (i * a_tiles + j) + 3] = (i + 1) * (a_tiles + 1) + j + 1;
			indices[6 * (i * a_tiles + j) + 4] = (i + 1) * (a_tiles + 1) + j;
			indices[6 * (i * a_tiles + j) + 5] = i * (a_tiles + 1) + j;
		}
	}

	Mesh mesh = Mesh(vertices, vertexCount, indices, indexCount);
	delete[] indices;
	delete[] vertices;
	return mesh;
}

Mesh Mesh::GenerateSphereMesh(unsigned int a_rows, unsigned int a_columns)
{
	const unsigned int vertexCount = (a_columns + 1) * (a_rows + 1);
	Vertex* vertices = new Vertex[vertexCount];
	for (unsigned int i = 0; i < a_rows + 1; ++i)
	{
		for (unsigned int j = 0; j < a_columns + 1; ++j)
		{
			glm::vec3 point;
			if (0 == i || a_rows == i)
			{
				point = glm::vec3(0, 0, (0 == i ? 1 : -1));
			}
			else if (a_columns == j)
			{
				point = vertices[i * (a_columns + 1)].position;
			}
			else
			{
				float azimuth = glm::two_pi<float>() * j / a_columns;
				float elevation = glm::pi<float>() * (0.5f - (float)i / a_rows);
				float c = glm::cos(elevation);
				point = glm::vec3(c * glm::cos(azimuth), c * glm::sin(azimuth), glm::sin(elevation));
			}
			vertices[i * (a_columns + 1) + j] = Vertex(point, point,
													   glm::vec2((float)j / a_columns, (float)i / a_rows));
		}
	}

	const unsigned int indexCount = a_columns * a_rows * 6;
	unsigned int* indices = new unsigned int[indexCount];
	for (unsigned int i = 0; i < a_rows; ++i)
	{
		for (unsigned int j = 0; j < a_columns; ++j)
		{
			indices[6 * (i * a_columns + j)] = i * (a_columns + 1) + j;
			indices[6 * (i * a_columns + j) + 1] = i * (a_columns + 1) + j + 1;
			indices[6 * (i * a_columns + j) + 2] = (i + 1) * (a_columns + 1) + j + 1;
			indices[6 * (i * a_columns + j) + 3] = (i + 1) * (a_columns + 1) + j + 1;
			indices[6 * (i * a_columns + j) + 4] = (i + 1) * (a_columns + 1) + j;
			indices[6 * (i * a_columns + j) + 5] = i * (a_columns + 1) + j;
		}
	}

	Mesh mesh = Mesh(vertices, vertexCount, indices, indexCount);
	delete[] indices;
	delete[] vertices;
	return mesh;
}