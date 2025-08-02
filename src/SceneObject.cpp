#include "SceneObject.h"


SceneObject::SceneObject(Model* model) : model(model) {}
SceneObject::SceneObject(Mesh* mesh) : mesh(mesh) {}

void SceneObject::draw(Shader& shader, Camera& camera) {
	shader.use();

	//update model matrix
	glm::mat4 model = getModelmatrix();
	shader.setMat4("model", model);

	//update normal matrix
	glm::mat3 normalMat = glm::transpose(glm::inverse(camera.getViewMatrix() * model));
	shader.setMat3("normalMat", normalMat);

	if (this->model) {
		this->model->draw(shader);
	}
	else {
		mesh->draw(shader);
	}
}

glm::mat4 SceneObject::getModelmatrix() const{
	glm::mat4 model(1.0f);

	//apply translation
	model = glm::translate(model, position);

	//apply rotations
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	//apply scale
	model = glm::scale(model, scale);

	return model;
}


