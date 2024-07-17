#version 330 core

layout (location = 0) in vec2 a_pos;

uniform mat4 projection;
uniform float scale;
uniform float point_size;

void main() {
	vec4 pos = projection * vec4(a_pos * scale, 0.0, 1.0);
	gl_Position = pos;
	gl_PointSize = point_size * scale * 0.5; // Note that point size determines diameter.
	}


