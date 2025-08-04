#pragma once

#include <iostream>

#include <glad/glad.h>
#include <random>

namespace Utils {
	GLuint textureFromFile(const char* path, const std::string& directory);
	float randomFloat(float min, float max);
}