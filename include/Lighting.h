#pragma once

struct SpotLight {
    float position[3] = {0.0f, 0.0f, 0.0f};
    float direction[3] = {0.0f, 0.0f, -1.0f};
    float color[3] = {1.0f, 0.9f, 0.7f};
    
    float intensity = 1.0f;
    float range = 10.0f;
    float innerCone = 12.5f;
    float outerCone = 17.5f;
    
    bool enabled = true;
};

struct DirectionalLight {
    float direction[3] = {-0.2f, -1.0f, -0.3f}; // Default: top-down angled light
    float color[3] = {0.8f, 0.9f, 1.0f}; // Cool white light
    float intensity = 0.6f;
    bool enabled = true;
    
    // Helper method to normalize direction
    void NormalizeDirection() {
        float length = sqrt(direction[0]*direction[0] + direction[1]*direction[1] + direction[2]*direction[2]);
        if (length > 0.0f) {
            direction[0] /= length;
            direction[1] /= length;
            direction[2] /= length;
        }
    }
};

struct AmbientLight {
    float color[3] = {0.1f, 0.05f, 0.15f};
    float intensity = 0.3f;
};

class LightingSystem {
public:
    SpotLight spotlight;
    DirectionalLight directional;
    AmbientLight ambient;
    
    LightingSystem() {
        // Initialize with good defaults
        directional.NormalizeDirection();
    }
    
    void SetFlashlightFromCamera(const float* cameraPos, const float* cameraFront) {
        spotlight.position[0] = cameraPos[0];
        spotlight.position[1] = cameraPos[1];
        spotlight.position[2] = cameraPos[2];
        
        spotlight.direction[0] = cameraFront[0];
        spotlight.direction[1] = cameraFront[1];
        spotlight.direction[2] = cameraFront[2];
    }
    
    void SetSpotlightIntensity(float intensity) {
        spotlight.intensity = intensity;
    }
    
    void ToggleSpotlight() {
        spotlight.enabled = !spotlight.enabled;
    }
    
    // New directional light controls
    void SetDirectionalDirection(float x, float y, float z) {
        directional.direction[0] = x;
        directional.direction[1] = y;
        directional.direction[2] = z;
        directional.NormalizeDirection();
    }
    
    void SetDirectionalIntensity(float intensity) {
        directional.intensity = intensity;
    }
    
    void SetDirectionalColor(float r, float g, float b) {
        directional.color[0] = r;
        directional.color[1] = g;
        directional.color[2] = b;
    }
    
    void ToggleDirectional() {
        directional.enabled = !directional.enabled;
    }
    
    // Preset lighting configurations for different moods
    void SetSunlightPreset() {
        SetDirectionalDirection(-0.3f, -0.8f, -0.5f);
        SetDirectionalColor(1.0f, 0.95f, 0.8f);
        SetDirectionalIntensity(0.8f);
        directional.enabled = true;
    }
    
    void SetMoonlightPreset() {
        SetDirectionalDirection(0.2f, -0.7f, 0.4f);
        SetDirectionalColor(0.6f, 0.7f, 1.0f);
        SetDirectionalIntensity(0.4f);
        directional.enabled = true;
    }
    
    void SetHorrorPreset() {
        SetDirectionalDirection(0.0f, -1.0f, 0.1f);
        SetDirectionalColor(0.4f, 0.3f, 0.6f);
        SetDirectionalIntensity(0.3f);
        directional.enabled = true;
        
        // Adjust ambient for horror atmosphere
        ambient.color[0] = 0.05f;
        ambient.color[1] = 0.02f;
        ambient.color[2] = 0.08f;
        ambient.intensity = 0.2f;
    }
};