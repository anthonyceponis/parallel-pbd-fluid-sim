#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Shader {
	uint32_t id;	

	Shader(const char *vShaderPath, const char *fShaderPath);	
	~Shader();

	void use();
	void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	void setFloat(const std::string &name, float value);
	void setVec3i(const std::string &name, glm::ivec3 &v);
	void setMat4(const std::string &name, glm::mat4 &matrix);
};
