#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

#include "solver.hpp"
#include "renderer.hpp"
#include "particles.hpp"
#include "shader.hpp"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void errorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << "\n";
}

int main() {
	const glm::vec2 screen_size(400, 800.0f);
	const float scale = 200.0f;
	const glm::vec2 world_size = screen_size / scale;	
	const std::string title = "Da best fluid sim";

	float prev_time = 0.0f, curr_time, dt;

	// GLFW: Init and config.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSetErrorCallback(errorCallback);

	// GLFW: Create window.
	GLFWwindow *window = glfwCreateWindow(screen_size.x, screen_size.y, title.c_str(), NULL, NULL);

	if (window == NULL) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync.

	// GLAD: Init.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to init GLAD\n";
		glfwTerminate();
		return -1;
	}

	// Bind callback function to execute on screen resize.
	// Gets called on window creation to init viewport.
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	const uint32_t particle_count = 10000; // Must be multiple of 100 for setup.
	Particles particles(particle_count);

	//Shader shader("circle.vs.glsl", "circle.fs.glsl");
	Solver solver(world_size, particles);
	Renderer renderer(screen_size, scale, particles);

	// Render loop.
	while (!glfwWindowShouldClose(window)) {

		// Update delta time.
		curr_time = glfwGetTime();
		dt = curr_time - prev_time;
		prev_time = curr_time;
		float fps = 1.0f / dt;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clearing colour.
		glClear(GL_COLOR_BUFFER_BIT);

		// Do stuff.
		solver.update(dt);
		renderer.drawParticles();

		glfwSetWindowTitle(window, (title + " " + std::to_string(fps)).c_str());

		//std::cout << fps << "\n";
		glfwSwapBuffers(window); // Double buffering.
		glfwPollEvents(); // Check for input events.

		// Cap at 60fps.
		while ((glfwGetTime() - curr_time) < 1.0f / 60.0f){};
	}

	glfwTerminate();
	return 0;

}

// Call function to reset OpenGL viewport on screen resize.
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Input handling function.
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
