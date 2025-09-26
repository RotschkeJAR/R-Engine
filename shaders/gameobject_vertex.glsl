#version 450 core

layout (location = 0) in vec2 I_vertex_position;

layout (location = 1) in vec3 I_instance_position;
layout (location = 2) in vec3 I_instance_color;
layout (location = 3) in uint I_instance_tex_id;
layout (location = 4) in vec2 I_instance_tex_coords;

layout (location = 0) out vec3 O_color;
layout (location = 1) flat out uint O_tex_id;
layout (location = 2) out vec2 O_tex_coords;

void main() {
	gl_Position = vec4(I_vertex_position + I_instance_position.xy, (I_instance_position.z + 1.0) * 0.5, 1.0);
	O_color = I_instance_color;
	O_tex_id = I_instance_tex_id;
	O_tex_coords = I_instance_tex_coords;
}
