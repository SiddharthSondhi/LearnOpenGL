#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

#include "Shader.h"


struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh {
private:
    GLuint VAO, VBO, EBO;
    GLsizei indicesSize;
    GLsizei vertexCount;
    std::vector<Texture> textures;

    void setUpAttributes(const std::vector<unsigned int>& attribSizes);

public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& attribSizes,const std::vector<Texture>& textures,const std::vector<unsigned int>& indices = {});
    void draw(Shader& shader);

};