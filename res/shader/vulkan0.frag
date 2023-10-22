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

layout(binding = 1) uniform sampler2D albe_tex;
layout(binding = 2) uniform sampler2D spec_tex;
layout(binding = 3) uniform sampler2D opac_tex;
layout(binding = 4) uniform sampler2D ambi_tex;
layout(binding = 5) uniform sampler2D norm_tex;
layout(binding = 6) uniform sampler2D emis_tex;

void main()
{ /*
   if (texture(opac_tex, frag_data.uv.xy).x == 0)
   {
       discard;
   }
   */

    position = vec4(frag_data.position, 1.0);
    normal = vec4(frag_data.normal, 1.0);
    albedo = vec4(frag_data.color, 1.0) * texture(albe_tex, frag_data.uv.xy);
    specular = albedo;
}