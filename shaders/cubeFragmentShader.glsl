#version 460 core
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

// structs
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 
  
struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct SpotLight{
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//uniforms
#define NR_POINT_LIGHTS 2  

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform Material material;
uniform bool enableFlashLight;

uniform mat4 model;
uniform mat4 view;

// function prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  


void main()
{
    //------------------------- doing all calculations in VIEW SPACE -----------------------
    // properties
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    vec3 result = vec3(0.0);
    
    // phase 1: Directional lighting
    result += calcDirLight(dirLight, norm, viewDir);

    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, fragPos, viewDir);  
    }

    // phase 3: Spot light
    if (enableFlashLight)
        result += calcSpotLight(spotLight, norm, fragPos, viewDir);    
    
    fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = -normalize(vec3(view * vec4(light.direction, 0.0)));

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    return (ambient + diffuse + specular);
}  


vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightPos = vec3(view * vec4(light.position, 1.0));
    vec3 lightDir = normalize(lightPos - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    
    return attenuation * (ambient + diffuse + specular);
} 

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 spotLightDir = normalize(-vec3(view * vec4(light.direction, 0.0)));
    vec3 lightPos = vec3(view * vec4(light.position, 1.0));
    vec3 fragToLightDir = normalize(lightPos - fragPos);

    // diffuse shading
    float diff = max(dot(normal, fragToLightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-fragToLightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //spotlight intensity calculation
    float theta = dot(fragToLightDir, normalize(spotLightDir));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  
    

    // attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    
    return attenuation * intensity * (ambient + diffuse + specular);
} 