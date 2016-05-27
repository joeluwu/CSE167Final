#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 camera;
uniform mat4 model;

out vec3 fragNormal;
out vec3 fragVert;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    
    fragVert = vec3(model * vec4(position, 1.0f));
    fragNormal = normalize( mat3( transpose(inverse(model)) ) * normal );
}
