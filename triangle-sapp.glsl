@ctype mat4 glm::mat4
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs vs
in vec4 position;
in vec4 color0;
in vec2 texCoords0;

out vec4 color;
out vec2 texCoords;

uniform vs_params {
    mat4 mvp;
};

void main() {
    gl_Position = mvp * position;
    color = color0;
    texCoords = texCoords0;
}
@end

@fs fs
in vec4 color;
in vec2 texCoords;
out vec4 frag_color;

uniform texture2D tex;
uniform sampler smp;

void main() {
    frag_color = texture(sampler2D(tex, smp), texCoords) * color;
}
@end

@program triangle vs fs



@vs vs_cube
uniform vs_params1 {
    mat4 mvp;
    vec3 box_position;
    vec3 box_size;
};

in vec3 position;
in vec4 color0;

out vec4 color;

void main() {
    gl_Position = mvp * vec4(box_position + position*box_size, 1.0f);
    color = color0;
}
@end

@fs fs_cube
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end

@program cube vs_cube fs_cube
