cmake_minimum_required(VERSION 3.0.0)
project(pbd VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

add_compile_options(-Ofast)

find_package(OpenGL REQUIRED)
find_package(CUDA REQUIRED)

cuda_add_library(cuda_functions STATIC solver_gpu.cu)

add_executable(pbd 
	main.cpp
	glad.c
	shader.cpp
	particles.cpp
	spatial_grid.cpp
	renderer.cpp
	solver.cpp
)

target_link_libraries(pbd
	glfw
	OpenGL::GL
	cuda_functions
	${CUDA_SOURCES}
)

target_include_directories(pbd PRIVATE ${CMAKE_SOURCE_DIR}/include)

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
