#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    GLuint id;
    std::string type;
};

class Mesh {
private:
    GLuint VAO, VBO, EBO;
    void setupMesh();

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int>  indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indeces, std::vector<Texture> textures);
    void draw(Shader& shader);

};