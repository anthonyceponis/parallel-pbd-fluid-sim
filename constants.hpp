#pragma once

// Most constants are named after this paper: 
// https://mmacklin.com/pbf_sig_preprint.pdf
namespace constants {	
	constexpr float h = 0.03f; // Smoothing radius.
	constexpr float h2 = h*h;
	constexpr uint32_t sub_steps = 8;
	constexpr float dt = 0.01f;	
	const glm::vec2 g(0.0f, -10.0f);
	constexpr float particle_radius = 0.01f; 	
	constexpr float particle_diameter = 2.0f * particle_radius;
	constexpr float rest_density = 1.0f / (particle_diameter * particle_diameter);
	constexpr float pi = 3.14159265f;
	constexpr float kernel_scale = 4.0f / (pi * h2 * h2 * h2 * h2);
	constexpr float eps = 0.0001f;
	constexpr float max_vel = 0.4f * particle_radius;
}
