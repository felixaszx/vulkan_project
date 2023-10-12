#version 450 core

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 specular;
layout(location = 2) out vec4 position;
layout(location = 3) out vec4 normal;

layout(location = 0) in struct
{
    vec3 position;
    vec3 normal;
    vec3 uv;
    vec3 color;
    vec3 specular;
} frag_data;

void main()
{
    albedo = vec4(1.0f,1.0f,1.0f, 1.0f);
    specular = vec4(frag_data.color, 1.0f);
    position = vec4(frag_data.position, 1.0);
    normal = vec4(frag_data.normal, 1.0);
}