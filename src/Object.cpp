#include "Object.h"
#include <numeric>

Object::Object(const std::vector<float>& vertices , const std::vector<unsigned int>& attribSizes, GLuint texture_diffuse1, GLuint texture_specular1) :
	vertices(vertices), attribSizes(attribSizes), texture_diffuse1(texture_diffuse1), texture_specular1(texture_specular1){

	setUpObject();
}

void Object::setUpObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// find stride size
	GLsizei stride = 0;
	for (auto i : attribSizes) {
		stride += sizeof(float) * i;
	}

	// figure out offsets
	std::vector<GLuint> offsets{ 0 };
	for (int i = 1; i < attribSizes.size(); i++) {
		offsets.push_back(attribSizes[i - 1] * sizeof(float) + offsets[i - 1]);
	}

	// attributes
	for (unsigned int i = 0; i < attribSizes.size(); i++) {
		glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, stride, (void*)(offsets[i]));
		glEnableVertexAttribArray(i);
	}

	glBindVertexArray(0);
}

void Object::draw(Shader& shader) {
	shader.use();

	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_diffuse1);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_specular1);

	shader.setInt("material.texture_diffuse1", 0);
	shader.setInt("material.texture_specular1", 1);

	// draw object
	glBindVertexArray(VAO);

	unsigned int totalAttribCount = std::accumulate(attribSizes.begin(), attribSizes.end(), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / totalAttribCount);
	glBindVertexArray(0);
}

void Object::setDiffuse(GLuint texture_diffuse1) {
	this->texture_diffuse1 = texture_diffuse1;
}

void Object::setSpecular(GLuint texture_specular1) {
	this->texture_specular1 = texture_specular1;
}
