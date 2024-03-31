#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shiniess;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec2 texCoord;
in vec3 position;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;

out vec4 fragColor;

void main()
{
    // Ambient
    vec3 abmient = directionalLight.ambient * texture(material.diffuse, texCoord).rgb;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-directionalLight.direction);
    float diff = max(dot(norm, lightDir), 0.f);
    vec3 diffuse = directionalLight.diffuse * diff * texture(material.diffuse, texCoord).rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shiniess);
    vec3 specular = directionalLight.specular * spec * texture(material.specular, texCoord).rgb;

    vec3 result = abmient + diffuse + specular;
    fragColor = vec4(result, 1.f);
}