#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 uv;
layout(location = 3) in vec3 color;
layout(location = 4) in mat4 instance_model;

layout(binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 proj;
}
ubo;

layout(location = 0) out struct
{
    vec3 position;
    vec3 normal;
    vec3 uv;
    vec3 color;
    vec3 specular;
} frag_data;

void main()
{
    vec4 world_position = instance_model * vec4(position, 1.0);
    frag_data.position = world_position.xyz;
    frag_data.normal = normalize(mat3(transpose(inverse(instance_model))) * normal);
    frag_data.uv = uv;
    frag_data.color = color;
    frag_data.specular = color;

    gl_Position = ubo.proj * ubo.view * world_position;
}