#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 view;
uniform mat4 perspective;

uniform vec3 ambient_light;
uniform vec3 diffuse_light;

out vec4 color;

void main() {

   // Intensity
   float I = dot(normalize(normal), normalize(diffuse_light));
   
   color = vec4(vec3(I, I, I) * ambient_light, 1.0);

   gl_Position = perspective * view * vec4(pos, 1.0);
}