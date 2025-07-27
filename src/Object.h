#pragma once

#include <glad/glad.h>

#include <vector>

#include "Shader.h"


class Object {
private:
	GLuint VAO, VBO;
	std::vector<unsigned int> attribSizes;
	std::vector<float> vertices;

	GLuint texture_diffuse1;
	GLuint texture_specular1;

	void setUpObject();

public:
	Object(const std::vector<float>& vertices, const std::vector<unsigned int>& attribSizes, GLuint texture_diffuse1, GLuint texture_specular1);
	void draw(Shader& shader);
	void setDiffuse(GLuint texture_diffuse1);
	void setSpecular(GLuint texture_specular1);


};