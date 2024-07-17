#include <bits/stdc++.h>

#include "solver.hpp"
#include "spatial_grid.hpp"
#include "particles.hpp"
#include "constants.hpp"

Solver::Solver(glm::vec2 _world_size, Particles &_particles) : 
particles(_particles),
world_size(_world_size) {
	this->spatial_grid = new SpatialGrid(this->particles);
	
	// Setup particles.
	const float spacing = constants::particle_radius * 1.5f;
	const glm::vec2 origin(0.1f, 3.0f);
	glm::vec2 pos = origin; 
	const uint32_t cols = 50;
	const uint32_t rows = this->particles.particle_count / cols;		
	
	for (uint32_t y = 0; y < rows; y++) {
		for (uint32_t x = 0; x < cols; x++) {
			this->particles.positions[y * cols + x] = pos;	
			this->particles.prev_positions[y * cols + x] = pos;	
			pos.x += spacing + constants::particle_radius;
		}
		pos.x = origin.x;
		pos.y -= spacing + constants::particle_radius;
	}
};

Solver::~Solver() { 
	delete this->spatial_grid;
 };

void Solver::update() {
	Solver::findNeighbours();	
	const float dt = constants::dt / constants::sub_steps; 

	for (uint32_t _ = 0; _ < constants::sub_steps; _++) {
		// Predict positions.
		for (uint32_t i = 0; i < this->particles.particle_count; i++) {
			this->particles.velocities[i] += constants::g * dt;
			this->particles.prev_positions[i] = this->particles.positions[i];
			this->particles.positions[i] += this->particles.velocities[i] * dt;
		}

		// Solve constraints.
		this->solveBoundaries();
		this->solveFluid();
		//this->solveFluidCuda();

		for (uint32_t i = 0; i < this->particles.particle_count; i++) {
			glm::vec2 v = this->particles.positions[i] - this->particles.prev_positions[i];
			const float vel = glm::length(v);
			
			// CFL. Essential for stablisation or everything will explode.
			if (vel > constants::max_vel) {
				v *= constants::max_vel / vel;
				this->particles.positions[i] = this->particles.prev_positions[i] + v;
			}
			this->particles.velocities[i] = v / dt;	
		}
	}
};

void Solver::findNeighbours() {
	this->spatial_grid->build();
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		const uint32_t query_size = this->spatial_grid->query(i);	
		this->particles.neighbour_counts[i] = query_size;		
	}
}

void Solver::solveBoundaries() {
	const float r = constants::particle_radius;

	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		this->particles.positions[i].x = glm::clamp(this->particles.positions[i].x, r, this->world_size.x - r);
		this->particles.positions[i].y = glm::clamp(this->particles.positions[i].y, r, this->world_size.y - r);	
	}
}

void Solver::solveFluid() {
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {	
		float rho = 0.0f;		
		float sum_grad2 = 0.0f;
		glm::vec2 grad_i(0.0f);

		const uint32_t query_size = this->particles.neighbour_counts[i];

		for (uint32_t j = 0; j < query_size; j++) {
			const uint32_t nb_i = this->particles.neighbours[i * this->particles.particle_count + j];
			glm::vec2 n = this->particles.positions[nb_i] - this->particles.positions[i];
			const float r2 = glm::dot(n,n);
						
			if (r2 > constants::h2) {
				this->particles.gradients[nb_i] = glm::vec2(0.0f);
			} else {
				// Normalise.
				const float r = sqrt(r2);
				if (r > 0.0f) n /= r;

				const float w = constants::h2 - r2;
				rho += constants::kernel_scale * w * w * w;
				const float grad = (constants::kernel_scale * 3.0f * w * w * (-2.0f * r)) / constants::rest_density;
				this->particles.gradients[nb_i] = n * grad;
				grad_i -= n * grad;
				sum_grad2 += grad * grad;	
			}
		}		
		
		sum_grad2 += glm::dot(grad_i,grad_i);
		const float c = rho / constants::rest_density - 1.0f;	
		const float lambda = -c / (sum_grad2 + constants::eps);

		for (uint32_t j = 0; j < query_size; j++) {
			const uint32_t nb_i = this->particles.neighbours[i * this->particles.particle_count + j];
			if (i == nb_i) {
				this->particles.positions[nb_i] += lambda * grad_i;
			} else {
				this->particles.positions[nb_i] += lambda * this->particles.gradients[nb_i];
			}
		}	
	}	
}
