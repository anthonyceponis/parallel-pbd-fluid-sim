#pragma once

#include <glm/glm.hpp>
#include "shader.hpp"
#include "particles.hpp"

struct Renderer {
	Shader shader;
	glm::vec2 screen_size;
	float scale;
	Particles &particles;
	Renderer(const glm::vec2 _screen_size, const float _scale, Particles &_particles);
	void drawParticles();
};
