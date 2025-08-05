#pragma once

#include <iostream>

#include <glad/glad.h>
#include <random>
#include <string>
#include <vector>

namespace Utils {
	GLuint textureFromFile(const char* path, const std::string& directory);
	float randomFloat(float min, float max);
	GLuint loadCubemap(std::vector<std::string> faces);
	GLuint loadCubemap(std::string folder);
}