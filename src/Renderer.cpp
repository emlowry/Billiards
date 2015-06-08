#include "Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <stdio.h>
#include <vector>

#define RENDERER_MAX_LIGHTS 10
#define RENDERER_VERTEX_SHADER_FILE "shaders/vertexShader.glsl"
#define RENDERER_FRAGMENT_SHADER_FILE "shaders/fragmentShader.glsl"
#define RENDERER_SHADOW_VERTEX_SHADER_FILE "shaders/vertexShader.glsl"
#define RENDERER_SHADOW_SHADER_FILE "shaders/fragmentShader.glsl"

namespace Renderer
{
	static bool sg_loaded = false;
	static unsigned int sg_shaderID;
	static unsigned int sg_modelLocation;
	static unsigned int sg_projectionViewLocation;
	static unsigned int sg_diffuseColorLocation;
	static unsigned int sg_specularColorLocation;
	static unsigned int sg_hasTextureLocation;
	static unsigned int sg_textureLocation;

	static unsigned int sg_cameraPositionLocation;
	static unsigned int sg_ambientLightLocation;

	static unsigned int sg_lightCountLocation;
	static unsigned int sg_lightColorLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightDirectionLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightPositionLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightPowerLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightAttenuationLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightAngleLocations[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightBlurLocations[RENDERER_MAX_LIGHTS];

	static glm::vec3 sg_cameraPosition = glm::vec3(0);
	static glm::vec3 sg_lightAmbient = glm::vec3(0);
	static Light sg_lights[RENDERER_MAX_LIGHTS];
	static unsigned int sg_lightCount = 0;

	struct Model
	{
		Mesh mesh;
		Texture texture;
		glm::mat4 modelMatrix;

		Model(const Mesh& a_mesh = Mesh(), const Texture& a_texture = Texture(),
			  const glm::mat4& a_modelMatrix = Engine::IDENTITY_MATRIX)
			: mesh(a_mesh), texture(a_texture), modelMatrix(a_modelMatrix) {}
	};
	static std::vector<Model> sg_renderQueue;
	static void Render(const Model& a_model);

	void QueueMesh(const Mesh& a_mesh, const Texture& a_texture, const glm::mat4& a_modelMatrix)
	{
		sg_renderQueue.push_back(Model(a_mesh, a_texture, a_modelMatrix));
	}
	void DrawQueuedMeshes()
	{
		for (Model model : sg_renderQueue)
			Render(model);
	}
	void ClearMeshQueue() { sg_renderQueue.clear(); }

	glm::vec3 GetCameraPosition() { return sg_cameraPosition; }
	void SetCameraPosition(const glm::vec3& a_position)
	{
		sg_cameraPosition = a_position;
	}

	glm::vec3 GetAmbientLight() { return sg_lightAmbient; }
	void SetAmbientLight(const glm::vec3& a_light)
	{
		sg_lightAmbient = a_light;
	}

	std::vector<Light> GetLights()
	{
		std::vector<Light> lights = std::vector<Light>(sg_lightCount);
		for (unsigned int i = 0; i < sg_lightCount; ++i)
			lights[i] = sg_lights[i];
		return lights;
	}
	void SetLights(const std::vector<Light>& a_lights)
	{
		for (unsigned int i = 0; i < a_lights.size() && i < RENDERER_MAX_LIGHTS; ++i)
			sg_lights[i] = a_lights[i];
		sg_lightCount = glm::min<unsigned int>(a_lights.size(), RENDERER_MAX_LIGHTS);
	}
	void AddLight(const Light& a_light)
	{
		if (sg_lightCount < RENDERER_MAX_LIGHTS)
		{
			sg_lights[sg_lightCount++] = a_light;
		}
	}
	void ClearLights()
	{
		sg_lightCount = 0;
	}

	static void UpdateLight(unsigned int i)
	{
		glUniform3fv(sg_lightColorLocations[i], 1, &(sg_lights[i].color[0]));
		glUniform3fv(sg_lightDirectionLocations[i], 1, &(sg_lights[i].direction[0]));
		glUniform3fv(sg_lightPositionLocations[i], 1, &(sg_lights[i].position[0]));
		glUniform1f(sg_lightPowerLocations[i], sg_lights[i].power);
		glUniform1f(sg_lightAttenuationLocations[i], sg_lights[i].attenuation);
		glUniform1f(sg_lightAngleLocations[i], sg_lights[i].angle);
		glUniform1f(sg_lightBlurLocations[i], sg_lights[i].blur);
	}
	static void SetUniforms()
	{
		if (sg_loaded)
		{
			glUseProgram(sg_shaderID);
			glUniformMatrix4fv(sg_projectionViewLocation, 1, false, glm::value_ptr(Engine::GetProjectionViewMatrix()));
			glUniform3fv(sg_cameraPositionLocation, 1, &sg_cameraPosition[0]);
			glUniform3fv(sg_ambientLightLocation, 1, &sg_lightAmbient[0]);
			for (unsigned int i = 0; i < sg_lightCount; ++i)
				UpdateLight(i);
			glUniform1ui(sg_lightCountLocation, sg_lightCount);
			glUniform1i(sg_textureLocation, 0);
		}
	}

	static unsigned int Compile(const char* const a_filename, GLenum a_shaderType)
	{
		// open file
		FILE* pFile;
		fopen_s(&pFile, a_filename, "rb");
		if (pFile == nullptr)
		{
			printf("Error: Unable to open '%s'\n", a_filename);
			return 0;
		}

		// get file size
		fseek(pFile, 0, SEEK_END);
		unsigned int size = (unsigned int)ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// copy file into char array
		char* source = new char[size + 1];
		memset(source, 0, size + 1);
		fread(source, sizeof(char), size, pFile);
		fclose(pFile);

		// create shader
		unsigned int shaderID = glCreateShader(a_shaderType);
		glShaderSource(shaderID, 1, (const char**)&source, 0);
		glCompileShader(shaderID);

		// clean up
		delete[] source;

		// check for success
		GLint success = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		// if compilation failed, 
		if (GL_FALSE == success)
		{
			// get the size of the error log
			int logSize = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);

			// copy the error log
			char* log = new char[logSize];
			glGetShaderInfoLog(shaderID, logSize, 0, log);

			// print error log
			printf("Error compiling %s:\n%s\n", a_filename, log);

			// clean up
			delete[] log;
			glDeleteShader(shaderID);

			// indicate failure
			return 0;
		}

		// otherwise, return shader handle
		return shaderID;
	}

	bool LoadShader()
	{
		if (sg_loaded)
			return true;

		// compile shaders
		unsigned int vertexShaderID = Compile(RENDERER_VERTEX_SHADER_FILE, GL_VERTEX_SHADER);
		if (0 == vertexShaderID)
			return false;
		unsigned int fragmentShaderID = Compile(RENDERER_FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
		if (0 == fragmentShaderID)
			return false;

		// attach shaders
		sg_shaderID = glCreateProgram();
		glAttachShader(sg_shaderID, vertexShaderID);
		glAttachShader(sg_shaderID, fragmentShaderID);

		// note attribute and output locations
		glBindAttribLocation(sg_shaderID, 0, "vertexPosition");
		glBindAttribLocation(sg_shaderID, 1, "vertexNormal");
		glBindAttribLocation(sg_shaderID, 2, "vertexTextureUV");
		glBindFragDataLocation(sg_shaderID, 0, "fragmentColor");

		// link program
		glLinkProgram(sg_shaderID);

		// cleanup
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		// check for success
		GLint success = GL_FALSE;
		glGetProgramiv(sg_shaderID, GL_LINK_STATUS, &success);

		// if successful,
		if (GL_TRUE == success)
		{
			// start finding and setting uniform values
			glUseProgram(sg_shaderID);

			sg_modelLocation = glGetUniformLocation(sg_shaderID, "model");
			sg_projectionViewLocation = glGetUniformLocation(sg_shaderID, "projectionView");
			sg_diffuseColorLocation = glGetUniformLocation(sg_shaderID, "diffuseColor");
			sg_specularColorLocation = glGetUniformLocation(sg_shaderID, "specularColor");
			sg_hasTextureLocation = glGetUniformLocation(sg_shaderID, "hasTexture");
			sg_textureLocation = glGetUniformLocation(sg_shaderID, "texture");

			sg_cameraPositionLocation = glGetUniformLocation(sg_shaderID, "cameraPosition");
			sg_ambientLightLocation = glGetUniformLocation(sg_shaderID, "lightAmbient");

			char buffer[50];
			for (unsigned int i = 0; i < RENDERER_MAX_LIGHTS; ++i)
			{
				sprintf_s(buffer, "lights[%d].color", i);
				sg_lightColorLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].direction", i);
				sg_lightDirectionLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].position", i);
				sg_lightPositionLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].power", i);
				sg_lightPowerLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].attenuation", i);
				sg_lightAttenuationLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].angle", i);
				sg_lightAngleLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
				sprintf_s(buffer, "lights[%d].blur", i);
				sg_lightBlurLocations[i] = glGetUniformLocation(sg_shaderID, buffer);
			}
			sg_lightCountLocation = glGetUniformLocation(sg_shaderID, "lightCount");

			// loading successful!
			sg_loaded = true;
			return true;
		}

		// otherwise, get the size of the error log
		int logSize = 0;
		glGetShaderiv(sg_shaderID, GL_INFO_LOG_LENGTH, &logSize);

		// copy the error log
		char* log = new char[logSize];
		glGetShaderInfoLog(sg_shaderID, logSize, 0, log);

		// print the error log
		printf("Error linking shader:\n%s\n", log);

		// cleanup
		delete[] log;
		glDeleteProgram(sg_shaderID);
		sg_shaderID = 0;

		// indicate failure
		sg_loaded = false;
		return false;
	}
	bool ShaderIsLoaded() { return sg_loaded; }
	void DestroyShader()
	{
		if (sg_loaded)
		{
			sg_loaded = false;
			glUseProgram(0);
			glDeleteProgram(sg_shaderID);
		}
	}

	void LoadMesh(Mesh& a_mesh, Mesh::Vertex* a_vertices, unsigned int a_vertexCount,
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

	static void Render(const Model& a_model)
	{
		// Set uniform values
		SetUniforms();
		glUniformMatrix4fv(sg_modelLocation, 1, false, glm::value_ptr(a_model.modelMatrix));
		glUniform4fv(sg_diffuseColorLocation, 1, &(a_model.texture.diffuseColor[0]));
		glUniform4fv(sg_specularColorLocation, 1, &(a_model.texture.specularColor[0]));
		if (GL_TRUE == glIsTexture(a_model.texture.imageID))
		{
			glUniform1ui(sg_hasTextureLocation, GL_TRUE);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, a_model.texture.imageID);
		}
		else
		{
			glUniform1ui(sg_hasTextureLocation, GL_FALSE);
		}

		// draw triangles
		glBindVertexArray(a_model.mesh.vertexArrayID);
		glDrawElements(GL_TRIANGLES, a_model.mesh.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	void DrawMesh(const Mesh& a_mesh, const Texture& a_texture, const glm::mat4& a_modelMatrix)
	{
		Render(Model(a_mesh, a_texture, a_modelMatrix));
	}

} // namespace Renderer
