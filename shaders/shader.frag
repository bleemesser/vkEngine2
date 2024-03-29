#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

void main() {
    outColor = vec4(inColor, 1.0) * texture(material, inTexCoord);
}