#pragma once

#include <glad/gl.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace gsbox {
	class Shader {
		public:
			Shader();
			void addSource(const fs::path& path, GLenum type);
			void use();
			~Shader();

		private:
			unsigned int id;
	};
}