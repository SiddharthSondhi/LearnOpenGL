#pragma once

#include <glad/glad.h>
#include <vector>

#include "Shader.h"
#include "Camera.h"


class Cubemap {
public:
	Cubemap(std::vector<float> vertices, GLuint texture);
	void draw(Shader& shader, Camera& camera);

	void setTexture(GLuint texture);

private:
	GLuint VBO, VAO;
	GLuint texture;
};