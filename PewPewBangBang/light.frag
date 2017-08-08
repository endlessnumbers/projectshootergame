#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specStrength = 0.5f;	//strength of specular component
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);	//negate lightDir because reflect wants vector to point away from light source
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specStrength * spec * lightColor;
	//calc dot product between viewDir and reflectDir then raise to power of 32
	//32 is shininess value; higher the value the more it reflects the light (highlight becomes smaller)
	
	vec3 result = (ambient + diffuse + specular) * objectColor;
	color = vec4(result, 1.0f);
}
