#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D specularSampler;


layout(location = 0) out vec4 outColor;

void main() {
    // outColor = vec4(aNormal,1.0);
    vec4 diffuse  = texture(texSampler, fragTexCoord);
    vec4 specular = texture(specularSampler, fragTexCoord);
    outColor = vec4(vec3(diffuse + specular), 1.0);
}