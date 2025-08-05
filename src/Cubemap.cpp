#include "Cubemap.h"

Cubemap::Cubemap(std::vector<float> vertices, GLuint texture) : texture(texture){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Cubemap::draw(Shader& shader, Camera& camera) {
	shader.use();

	glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
	shader.setMat4("view", view);

	shader.setInt("skybox", 0);

	glDepthMask(GL_FALSE);
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	glBindVertexArray(0);

}

void Cubemap::setTexture(GLuint texture) {
	this->texture = texture;
}



