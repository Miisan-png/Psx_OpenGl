#pragma once

#include <glad/glad.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Shader.h"

struct Particle {
    float position[3];
    float velocity[3];
    float life;
    float maxLife;
    float size;
    float alpha;
    
    bool active = false;
};

class ParticleSystem {
public:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    Shader* particleShader;
    
    float spawnRate = 50.0f;
    float lastSpawn = 0.0f;
    
    // Made spawn box smaller and closer to camera
    float spawnBox[6] = {-5.0f, 5.0f, 0.0f, 3.0f, -5.0f, 5.0f};
    
    ParticleSystem(int maxParticles = 1000) {
        particles.resize(maxParticles);
        srand(time(nullptr));
        setupRenderData();
        createShader();
        
        // Spawn some initial particles for immediate visibility
        for (int i = 0; i < 100; i++) {
            float pos[3] = {0.0f, 1.0f, 0.0f}; // Spawn at eye level
            SpawnParticle(pos);
        }
    }
    
    void Update(float deltaTime, const float* cameraPos) {
        lastSpawn += deltaTime;
        
        if (lastSpawn > 1.0f / spawnRate) {
            SpawnParticle(cameraPos);
            lastSpawn = 0.0f;
        }
        
        for (auto& particle : particles) {
            if (!particle.active) continue;
            
            particle.position[0] += particle.velocity[0] * deltaTime;
            particle.position[1] += particle.velocity[1] * deltaTime;
            particle.position[2] += particle.velocity[2] * deltaTime;
            
            particle.life -= deltaTime;
            particle.alpha = particle.life / particle.maxLife;
            
            if (particle.life <= 0.0f) {
                particle.active = false;
            }
        }
    }
    
    void Render(const float* view, const float* projection, const float* cameraPos) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        
        particleShader->use();
        particleShader->setMat4("view", view);
        particleShader->setMat4("projection", projection);
        particleShader->setVec3("cameraPos", cameraPos[0], cameraPos[1], cameraPos[2]);
        
        glBindVertexArray(VAO);
        
        int renderedCount = 0;
        for (const auto& particle : particles) {
            if (!particle.active) continue;
            
            particleShader->setVec3("particlePos", particle.position[0], particle.position[1], particle.position[2]);
            particleShader->setFloat("particleSize", particle.size);
            particleShader->setFloat("particleAlpha", particle.alpha);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            renderedCount++;
        }
        
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        
        // Debug output (remove this later)
        static float debugTimer = 0.0f;
        debugTimer += 0.016f; // assume 60fps
        if (debugTimer > 2.0f) {
            std::cout << "Rendering " << renderedCount << " particles" << std::endl;
            debugTimer = 0.0f;
        }
    }
    
private:
    void SpawnParticle(const float* cameraPos) {
        for (auto& particle : particles) {
            if (particle.active) continue;
            
            // Spawn closer to camera and at visible heights
            particle.position[0] = cameraPos[0] + RandomFloat(spawnBox[0], spawnBox[1]);
            particle.position[1] = cameraPos[1] + RandomFloat(spawnBox[2], spawnBox[3]);
            particle.position[2] = cameraPos[2] + RandomFloat(spawnBox[4], spawnBox[5]);
            
            particle.velocity[0] = RandomFloat(-0.5f, 0.5f);
            particle.velocity[1] = RandomFloat(-0.2f, 0.2f);
            particle.velocity[2] = RandomFloat(-0.5f, 0.5f);
            
            particle.life = RandomFloat(5.0f, 10.0f);
            particle.maxLife = particle.life;
            particle.size = RandomFloat(0.02f, 0.08f); // Tiny dust particles
            particle.alpha = 1.0f;
            particle.active = true;
            
            break;
        }
    }
    
    float RandomFloat(float min, float max) {
        return min + (max - min) * ((float)rand() / RAND_MAX);
    }
    
    void setupRenderData() {
        // Bigger quad for particles
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
        };
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
    }
    
    void createShader() {
        std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            
            uniform mat4 view;
            uniform mat4 projection;
            uniform vec3 particlePos;
            uniform float particleSize;
            uniform vec3 cameraPos;
            
            out vec2 TexCoord;
            out vec3 worldPos;
            
            void main() {
                // Billboard the particle to always face camera
                vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
                vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);
                
                vec3 worldPos = particlePos + cameraRight * aPos.x * particleSize + cameraUp * aPos.y * particleSize;
                
                TexCoord = aPos.xy * 0.5 + 0.5; // Convert from -1,1 to 0,1
                
                gl_Position = projection * view * vec4(worldPos, 1.0);
            }
        )";
        
        std::string fragmentSource = R"(
            #version 330 core
            in vec2 TexCoord;
            out vec4 FragColor;
            
            uniform float particleAlpha;
            
            void main() {
                // Create sharp circular particle - no blurriness
                vec2 center = vec2(0.5, 0.5);
                float dist = distance(TexCoord, center);
                
                // Sharp cutoff for crisp edges
                if (dist > 0.4) discard;
                
                // Less smoothing for sharper look
                float alpha = 1.0 - (dist / 0.4);
                alpha = alpha * alpha; // Sharper falloff
                
                // Crisp white dust particles
                vec3 color = vec3(1.0, 1.0, 1.0);
                FragColor = vec4(color, alpha * particleAlpha * 0.6);
            }
        )";
        
        particleShader = new Shader(vertexSource, fragmentSource, true);
    }
};