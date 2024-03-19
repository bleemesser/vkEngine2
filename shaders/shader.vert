#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} cameraData;

layout(std140, set = 0, binding = 1) readonly buffer storageBuffer {
    mat4 model[];
} objectData;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 vertexTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main()
{
    gl_Position = cameraData.viewProjection * objectData.model[gl_InstanceIndex] * vec4(vertPos, 1.0);
    outColor = vertColor;
    outTexCoord = vertexTexCoord;
}