#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;
    void setMat3(const std::string& name, glm::mat3 value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);

};
