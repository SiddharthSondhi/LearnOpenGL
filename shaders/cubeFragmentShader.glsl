#version 460 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec3 lightPosView;
in vec2 texCoords;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
}; 
  
struct Light {  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Material material;
uniform Light light; 

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosView - fragPos); 
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

    //specular
    vec3 camDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(camDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;  

    // final color
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}