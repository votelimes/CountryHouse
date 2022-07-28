#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out float Visibility;

uniform float density;
const float gradient = 1.0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    FragPos = vec3(worldPosition);

    vec4 positionRelativeToCam = view * worldPosition;
    float distance = length(positionRelativeToCam.xyz);
    Visibility = exp(-pow(distance*density, gradient));
    Visibility = clamp(Visibility, 0.0, 1.0);

    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}