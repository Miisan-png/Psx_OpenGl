#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform float screenWidth;
uniform float screenHeight;
uniform float renderWidth;
uniform float renderHeight;

uniform float scanlineIntensity = 0.8;
uniform float scanlineFrequency = 2.0;
uniform float grainIntensity = 0.6;
uniform float grainAmount = 0.08;
uniform float aberrationIntensity = 0.5;
uniform float aberrationStrength = 0.002;
uniform float vignetteIntensity = 0.7;
uniform float vignetteStrength = 0.8;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec2 uv = TexCoord;
    vec3 color = texture(screenTexture, uv).rgb;
    
    // PSX-style color quantization (chunky colors)
    color = floor(color * 64.0) / 64.0;
    
    // Scanlines
    float scanline = sin(uv.y * renderHeight * scanlineFrequency) * 0.04 * scanlineIntensity;
    color -= scanline;
    
    // Film grain
    float grain = random(uv + time * 0.01) * grainAmount * grainIntensity;
    color += grain;
    
    // Chromatic aberration
    float aberration = aberrationStrength * aberrationIntensity;
    color.r = texture(screenTexture, uv + vec2(aberration, 0.0)).r;
    color.b = texture(screenTexture, uv - vec2(aberration, 0.0)).b;
    
    // Vignette
    vec2 center = uv - 0.5;
    float vignette = 1.0 - dot(center, center) * vignetteStrength * vignetteIntensity;
    color *= vignette;
    
    // Slight flicker
    float flicker = 0.98 + 0.02 * sin(time * 60.0);
    color *= flicker;
    
    // Slightly reduce overall brightness for horror mood
    color *= 0.9;
    
    FragColor = vec4(color, 1.0);
}