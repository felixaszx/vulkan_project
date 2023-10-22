#version 450 core

layout(location = 0) out vec4 light_result;

layout(input_attachment_index = 0, binding = 0) uniform subpassInput albedo;
layout(input_attachment_index = 1, binding = 1) uniform subpassInput specular;
layout(input_attachment_index = 2, binding = 2) uniform subpassInput position;
layout(input_attachment_index = 3, binding = 3) uniform subpassInput normal;

void main()
{
    vec3 frag_color = subpassLoad(albedo).rgb;
    vec3 frag_spec = subpassLoad(specular).rgb;
    vec3 frag_pos = subpassLoad(position).rgb;
    vec3 frag_normal = subpassLoad(normal).rgb;

    light_result = vec4(1.0,1.0,0.0, 1.0);
}