 
#version 430
//in vec3 color;
in vec3 worldNorm;
in vec3 worldPos;

struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int coeff;
};

uniform PointLight pointLights[2];
uniform vec3 eyePos;

out vec4 frag_color;


vec3 CalculateIntensity(PointLight light, vec3 worldNormal, vec3 worldPosition){
	vec3 lightDir = normalize(light.position - worldPosition);
	float diff = max(dot(worldNormal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, worldNormal);
	vec3 viewDir = normalize(eyePos - worldPosition);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),light.coeff);

	vec3 ambient = light.ambient;
	vec3 specular = light.specular*spec;
	vec3 diffuse = light.diffuse*diff;

	return (ambient+specular+diffuse);
}

void main() {
    vec3 worldNormFrag = normalize(worldNorm);
	vec3 result = vec3(0.0,0.0,0.0);
    for(int i=0;i<2;i++){
	//intensity += CalculateIntensity(lightPos[i]);
	result += CalculateIntensity(pointLights[i], worldNormFrag, worldPos);
    }


  frag_color = vec4(result, 1.0f);
}