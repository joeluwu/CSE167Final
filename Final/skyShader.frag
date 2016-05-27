#version 330 core

in vec3 TexCoords;  // Direction vector representing a 3D texture coordinate
out vec4 color;    // Cubemap texture sampler

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, TexCoords);
}