#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "./in.glsl"

// layout(early_fragment_tests) in;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 fragPos;

layout(set=0,binding = 0 ) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 viewPos;
    vec3 lightPosition;
    vec3 lightDiffuse;
} ubo;
layout(set=1, binding = 0) uniform sampler2D texSampler;
layout(set=1, binding = 1) uniform sampler2D specularSampler;


layout(location = 0) out vec4 outColor;

void main() {
    //ambient
    float ambientStrength = 0.24;
    vec3 ambient = ambientStrength * vec3(texture(texSampler, fragTexCoord));

    //diffuse
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 lightDir = normalize(ubo.lightPosition - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = diffF(norm,lightDir);
    vec3 diffuse  =ubo.lightDiffuse * diff * vec3(texture(texSampler, fragTexCoord));
    
    //specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(halfwayDir, norm), 0.0), 32);
    float specularStrength = 0.5;
    vec3 specular = specularStrength * spec  * vec3(texture(specularSampler, fragTexCoord));
    
    outColor = vec4(diffuse + specular + ambient , 1.0);
    // outColor = vec4(0.0,1.0,0.0,1.0);
}