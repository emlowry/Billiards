#include "Texture.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* const a_imageFileName,
				 const glm::vec4& a_diffuseColor,
				 const glm::vec4& a_specularColor)
: imageID(0), diffuseColor(a_diffuseColor), specularColor(a_specularColor)
{
	// read image
	int width = 0;
	int height = 0;
	int format = 0;
	unsigned char* data = stbi_load(a_imageFileName, &width, &height, &format, STBI_rgb_alpha);// STBI_default);

	// create OpenGL texture
	glGenTextures(1, &imageID);

	// load data into texture
	glBindTexture(GL_TEXTURE_2D, imageID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// set wrapping and filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// clean up
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
}

void Texture::Destroy()
{
	if (GL_TRUE == glIsTexture(imageID))
		glDeleteTextures(1, &imageID);
	imageID = 0;
}