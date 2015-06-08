#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Texture
{
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	unsigned int imageID;

	Texture(const glm::vec4& a_diffuseColor = glm::vec4(1),
			const glm::vec4& a_specularColor = glm::vec4(0.5))
		: imageID(0), diffuseColor(a_diffuseColor), specularColor(a_specularColor) {}
	Texture(unsigned int a_imageID, const glm::vec4& a_diffuseColor = glm::vec4(1),
			const glm::vec4& a_specularColor = glm::vec4(0.5))
		: imageID(a_imageID), diffuseColor(a_diffuseColor), specularColor(a_specularColor) {}
	Texture(const char* const a_imageFileName,
			const glm::vec4& a_diffuseColor = glm::vec4(1),
			const glm::vec4& a_specularColor = glm::vec4(0.5));

	void Destroy();
};

#endif	// _TEXTURE_H_
