#include "Mesh.h"
#include <numeric>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& attribSizes, const std::vector<Texture>& textures, const std::vector<unsigned int>& indices) :
	textures (textures){
	
	indicesSize = indices.size();
	GLsizei vertexSize = std::accumulate(attribSizes.begin(), attribSizes.end(), 0);
	vertexCount = vertices.size() / vertexSize;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// set up EBO if indeces provided
	if (indicesSize != 0) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	setUpAttributes(attribSizes);
	glBindVertexArray(0);
}


void Mesh::setUpAttributes(const std::vector<unsigned int>& attribSizes) {
	glBindVertexArray(VAO);

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


void Mesh::draw(Shader& shader) {
	shader.use();

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding

		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);

	// Use EBO if indices provided
	if (indicesSize != 0) {
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	glBindVertexArray(0);
}