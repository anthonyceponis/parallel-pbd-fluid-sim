#include "renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shader.hpp"
#include "particles.hpp"
#include "solver.hpp"

Renderer::Renderer(const glm::vec2 _screen_size, const float _scale, Particles &_particles) : 
screen_size(_screen_size), 
scale(_scale),
shader("shaders/circle.vs.glsl", "shaders/circle.fs.glsl"),
particles(_particles){};

void Renderer::drawParticles() {
	// Vertex data : [pos_x, pos_y].
	float vertex_data[this->particles.particle_count * 2];

	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		vertex_data[2 * i] = this->particles.positions[i].x;	
		vertex_data[2 * i + 1] = this->particles.positions[i].y;	
	}

	uint32_t vao, vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

	glEnableVertexAttribArray(0);

	glEnable(GL_PROGRAM_POINT_SIZE); // Enable point size control in shader.

	glm::mat4 projection = glm::ortho(0.0f, this->screen_size.x, 0.0f, this->screen_size.y, 0.0f, 1.0f);

	this->shader.use();
	shader.setMat4("projection", projection);
	shader.setFloat("scale", 200.0f);
	shader.setFloat("point_size", 2 * constants::particle_radius); // x2 since point_size is the diameter.

	glDrawArrays(GL_POINTS, 0, this->particles.particle_count);
};

