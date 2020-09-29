#version 450
#extension GL_ARB_separate_shader_objects : enable


#include "./in.glsl"

layout(set= 0, binding = 0, std140 ) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 viewPos;
    vec3 lightPositon;
    vec3 lightDiffuse;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 aNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 fragPos;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    normal = mat3(transpose(inverse(ubo.model))) * aNormal;
}

