#version 460 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec3 lightPosView;
  
uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosView - fragPos); 
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 0.5;
    vec3 camDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(camDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    // final color
    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0);
}