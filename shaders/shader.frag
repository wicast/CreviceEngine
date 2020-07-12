#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 fragPos;

layout(binding = 0,std140 ) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 viewPos;
    vec3 lightPosition;
    vec3 lightDiffuse;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D specularSampler;


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

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  =ubo.lightDiffuse * diff * vec3(texture(texSampler, fragTexCoord));
    
    //specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(halfwayDir, norm), 0.0), 32);
    float specularStrength = 0.5;
    vec3 specular = specularStrength * spec  * vec3(texture(specularSampler, fragTexCoord));
    
    outColor = vec4(diffuse + specular + ambient , 1.0);
}