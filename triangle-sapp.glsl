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