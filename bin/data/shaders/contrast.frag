#version 410

// contrast
// fragment shader

uniform sampler2DRect tex0;
uniform float u_contrastAmount = 0.0;

in vec2 texCoordVarying;
out vec4 outputColor;

mat4 contrastMatrix(float contrast) {
    float t = (1.0 - contrast) / 2.0;
    
    return mat4( contrast, 0, 0, 0,
                 0, contrast, 0, 0,
                 0, 0, contrast, 0,
                 t, t, t, 1 );
}

void main() {
    outputColor = contrastMatrix(u_contrastAmount) * texture(tex0, texCoordVarying);
}
