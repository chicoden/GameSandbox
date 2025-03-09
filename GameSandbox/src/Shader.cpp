#include <fstream>
#include "Shader.hpp"

using namespace gsbox;

Shader::Shader() {
	id = glCreateProgram();
}

void Shader::addSource(const fs::path& path, GLenum type) {
	std::ifstream file(path);
	if (!file.is_open()) {
		// TODO
	}
}

void Shader::use() {
	glUseProgram(id);
}

Shader::~Shader() {
	glDeleteProgram(id);
}