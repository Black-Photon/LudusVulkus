#version 450

layout(binding = 0) uniform Transformations {
    mat4 model;
    mat4 view;
    mat4 projection;
} transformations;

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 frag_tex_coord;

void main() {
    gl_Position = transformations.projection * transformations.view * transformations.model * vec4(in_position, 0.0, 1.0);
    out_color = in_color;
    frag_tex_coord = in_tex_coord;
}