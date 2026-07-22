#version 450 core

layout (location = 0) in vec2 uv;
layout (location = 1) flat in uint charIndex;

layout (location = 0) out vec4 color;

layout (set = 0, binding = 0) uniform sampler2DArray charTextures;

layout (set = 1, binding = 0) uniform Metadata {
	vec4 textColor;
} meta;

void main() {
	color = texture(charTextures, vec3(uv, float(charIndex))) * meta.textColor;
	if (color.w < 1.0)
		discard;
}
