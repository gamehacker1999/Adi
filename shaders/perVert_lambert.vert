#version 430

uniform mat4 modelMat; 
uniform mat4 viewMat;
uniform mat4 projMat; 
uniform vec3 lightPos;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

out vec3 color;

void main() {
  
  vec3 lightDir = normalize(lightPos - vertex_position);
  float intensity = max(dot(vertex_normal, lightDir), 0.0);
  color = intensity * vec3(1.0, 1.0, 0.0);
	
  gl_Position = projMat * viewMat * modelMat * vec4(vertex_position, 1.0f);
}




