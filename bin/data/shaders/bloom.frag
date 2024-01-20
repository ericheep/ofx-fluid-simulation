#version 410

// bloom
// fragment shader

uniform sampler2DRect tex0;
uniform float bloom_spread = 1.0;
uniform float bloom_intensity = 2.0;

in vec2 texCoordVarying;
out vec4 outputColor;

void main() {
    float uv_x = texCoordVarying.x;
    float uv_y = texCoordVarying.y;
    
    vec4 sum = vec4(0.0);
    for (int n = 0; n < 17; ++n) {
        uv_y = texCoordVarying.y + (bloom_spread * float(n - 8.0));
        
        vec4 h_sum = vec4(0.0, 0.0, 0.0, 0.0);
        h_sum += texture(tex0, vec2(uv_x - (8.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - (7.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - (6.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - (5.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - (3.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - (2.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x - bloom_spread, uv_y));
        h_sum += texture(tex0, vec2(uv_x, uv_y));
        h_sum += texture(tex0, vec2(uv_x + bloom_spread, uv_y));
        h_sum += texture(tex0, vec2(uv_x + (2.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (3.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (4.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (5.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (6.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (7.0 * bloom_spread), uv_y));
        h_sum += texture(tex0, vec2(uv_x + (8.0 * bloom_spread), uv_y));
        
        sum += h_sum / 17.0;
    }
    
    vec4 bloom = texture(tex0, texCoordVarying) + ((sum / 17.0) * bloom_intensity);
    outputColor = bloom;
}
