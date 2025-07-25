#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform float screenWidth;
uniform float screenHeight;

void main() {
    vec2 uv = TexCoord;
    vec2 cc = uv - 0.5;
    float dist = dot(cc, cc) * 0.2;
    uv = uv + cc * (1.0 + dist) * dist;
    vec3 color = texture(screenTexture, uv).rgb;
    
    float separation = 0.001;
    color.r = texture(screenTexture, uv + vec2(separation, 0.0)).r;
    color.g = texture(screenTexture, uv).g;
    color.b = texture(screenTexture, uv - vec2(separation, 0.0)).b;
    
    float scanline = sin(uv.y * screenHeight * 2.0) * 0.1;
    color -= scanline;
    vec2 center = uv - 0.5;
    float vignette = 1.0 - dot(center, center) * 1.2;
    color *= vignette;
    
    float noise = fract(sin(dot(uv + time * 0.001, vec2(12.9898, 78.233))) * 43758.5453) * 0.05;
    color += noise;
    
    FragColor = vec4(color, 1.0);
}