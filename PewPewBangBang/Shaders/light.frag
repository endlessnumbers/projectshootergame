#version 430
struct Material {
	sampler2D specular;
	float shininess;
	sampler2D diffuse;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;
uniform Light light;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));	//forms of lighting should have diff effects
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);	//negate lightDir because reflect wants vector to point away from light source
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	//calc dot product between viewDir and reflectDir then raise to power of shininess
	//shininess value; higher the value the more it reflects the light (highlight becomes smaller)
	
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f);
}