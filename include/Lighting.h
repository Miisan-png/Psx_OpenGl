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

struct AmbientLight {
    float color[3] = {0.1f, 0.05f, 0.15f};
    float intensity = 0.3f;
};

class LightingSystem {
public:
    SpotLight spotlight;
    AmbientLight ambient;
    
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
};