#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform float renderHeight;
uniform float scanlineIntensity = 1.0;
uniform float scanlineFrequency = 2.0;

void main() {
    vec2 uv = TexCoord;
    vec3 color = texture(screenTexture, uv).rgb;
    float scanline = sin(uv.y * renderHeight * scanlineFrequency) * 0.15 * scanlineIntensity;
    color -= scanline;
    float line = floor(uv.y * renderHeight * 0.5);
    float alternating = mod(line, 2.0) * 0.1 * scanlineIntensity;
    color -= alternating;
    
    FragColor = vec4(color, 1.0);
}