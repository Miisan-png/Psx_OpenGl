#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "ShaderManager.h"
#include <vector>
#include <string>

enum class EffectType {
    PSX_RETRO,
    SCANLINES,
    FILM_GRAIN,
    CHROMATIC_ABERRATION,
    VIGNETTE,
    CRT_MONITOR
};

struct EffectSettings {
    EffectType type;
    bool enabled = true;
    float intensity = 1.0f;
    
    // Effect-specific parameters
    float scanlineFrequency = 2.0f;
    float grainAmount = 0.08f;
    float aberrationStrength = 0.002f;
    float vignetteStrength = 0.8f;
};

class PostProcessEffect {
public:
    unsigned int framebuffer;
    unsigned int colorTexture;
    unsigned int depthTexture;
    unsigned int quadVAO, quadVBO;
    
    std::vector<EffectSettings> effects;
    std::string currentShaderName = "psx_retro";
    
    int width, height;
    
    PostProcessEffect(int w, int h) : width(w), height(h) {
        setupFramebuffer();
        setupQuad();
        setupDefaultEffects();
        loadShaders();
    }
    
    void AddEffect(EffectType type, float intensity = 1.0f) {
        EffectSettings effect;
        effect.type = type;
        effect.intensity = intensity;
        effects.push_back(effect);
        updateShader();
    }
    
    void RemoveEffect(EffectType type) {
        effects.erase(
            std::remove_if(effects.begin(), effects.end(),
                [type](const EffectSettings& effect) { return effect.type == type; }),
            effects.end()
        );
        updateShader();
    }
    
    void SetEffectIntensity(EffectType type, float intensity) {
        for (auto& effect : effects) {
            if (effect.type == type) {
                effect.intensity = intensity;
                break;
            }
        }
    }
    
    void ToggleEffect(EffectType type) {
        for (auto& effect : effects) {
            if (effect.type == type) {
                effect.enabled = !effect.enabled;
                break;
            }
        }
    }
    
    void BeginRender() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void EndRender() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void RenderToScreen(int screenWidth, int screenHeight) {
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        
        Shader* shader = ShaderManager::Instance().GetShader(currentShaderName);
        if (!shader) return;
        
        shader->use();
        setShaderUniforms(shader, screenWidth, screenHeight);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        shader->setInt("screenTexture", 0);
        
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
    }
    
    void Resize(int w, int h) {
        width = w;
        height = h;
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    
    ~PostProcessEffect() {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &colorTexture);
        glDeleteTextures(1, &depthTexture);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }

private:
    void setupDefaultEffects() {
        AddEffect(EffectType::PSX_RETRO, 1.0f);
        AddEffect(EffectType::SCANLINES, 0.8f);
        AddEffect(EffectType::FILM_GRAIN, 0.6f);
        AddEffect(EffectType::CHROMATIC_ABERRATION, 0.5f);
        AddEffect(EffectType::VIGNETTE, 0.7f);
    }
    
    void loadShaders() {
        ShaderManager& sm = ShaderManager::Instance();
        
        sm.LoadShader("psx_retro", "shaders/screen.vert", "shaders/psx_retro.frag");
        sm.LoadShader("scanlines", "shaders/screen.vert", "shaders/scanlines.frag");
        sm.LoadShader("crt_monitor", "shaders/screen.vert", "shaders/crt_monitor.frag");
    }
    
    void updateShader() {
        
    }
    
    void setShaderUniforms(Shader* shader, int screenWidth, int screenHeight) {
        shader->setFloat("time", glfwGetTime());
        shader->setFloat("screenWidth", (float)screenWidth);
        shader->setFloat("screenHeight", (float)screenHeight);
        shader->setFloat("renderWidth", (float)width);
        shader->setFloat("renderHeight", (float)height);
        
        for (const auto& effect : effects) {
            if (!effect.enabled) continue;
            
            switch (effect.type) {
                case EffectType::SCANLINES:
                    shader->setFloat("scanlineIntensity", effect.intensity);
                    shader->setFloat("scanlineFrequency", effect.scanlineFrequency);
                    break;
                case EffectType::FILM_GRAIN:
                    shader->setFloat("grainIntensity", effect.intensity);
                    shader->setFloat("grainAmount", effect.grainAmount);
                    break;
                case EffectType::CHROMATIC_ABERRATION:
                    shader->setFloat("aberrationIntensity", effect.intensity);
                    shader->setFloat("aberrationStrength", effect.aberrationStrength);
                    break;
                case EffectType::VIGNETTE:
                    shader->setFloat("vignetteIntensity", effect.intensity);
                    shader->setFloat("vignetteStrength", effect.vignetteStrength);
                    break;
            }
        }
    }

private:
    void setupFramebuffer() {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        
        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
        
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void setupQuad() {
        float quadVertices[] = {
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
};