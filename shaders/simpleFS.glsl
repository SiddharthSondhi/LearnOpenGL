#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture_diffuse1;

void main(){
   FragColor = texture(texture_diffuse1, texCoord);
} 