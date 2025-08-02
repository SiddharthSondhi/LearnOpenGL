#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"


class SceneObject {
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	Model* model = nullptr;
	Mesh* mesh = nullptr;

	SceneObject(Model* model);
	SceneObject(Mesh* mesh);

	void draw(Shader& shader, Camera& camera);

private:
	glm::mat4 getModelmatrix() const;
};