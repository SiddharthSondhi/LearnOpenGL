#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;



void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    normal = normalMat * aNormal;
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    texCoords = aTexCoords;
} 