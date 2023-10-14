#version 450 core

vec3 positions[6] = {
    // first triangle
    vec3(-1.0f, -1.0f, 0.0f), // top left
    vec3(1.0f, 1.0f, 0.0f),   // bottom right
    vec3(1.0f, -1.0f, 0.0f),  // up right
                              // second triangle
    vec3(-1.0f, -1.0f, 0.0f), // top left
    vec3(-1.0f, 1.0f, 0.0f),  // bottom left
    vec3(1.0f, 1.0f, 0.0f)    // bottom right
};

vec2 uv[6] = {
    // first triangle
    vec2(0.0f, 1.0f), // top left
    vec2(1.0f, 0.0f), // bottom right
    vec2(1.0f, 1.0f), // up right
                      // second triangle
    vec2(0.0f, 1.0f), // top left
    vec2(0.0f, 0.0f), // bottom left
    vec2(1.0f, 0.0f)  // bottom right
};

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}