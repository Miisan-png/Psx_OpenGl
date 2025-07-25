#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

class Skybox {
public:
    Shader* skyboxShader;
    unsigned int skyboxVAO, skyboxVBO;
    float totalTime;
    
    Skybox() : skyboxShader(nullptr), skyboxVAO(0), skyboxVBO(0), totalTime(0.0f) {}
    
    bool Initialize() {
        setupSkyboxGeometry();
        createSkyboxShader();
        return skyboxShader != nullptr;
    }
    
    void Update(float deltaTime) {
        totalTime += deltaTime;
    }
    
    void Render(Camera& camera, float aspectRatio) {
        if (!skyboxShader) return;
        
        glDepthMask(GL_FALSE);
        
        skyboxShader->use();
        
        // Set up view matrix without translation
        float view[16];
        camera.GetViewMatrix(view);
        view[12] = view[13] = view[14] = 0.0f;
        
        float projection[16];
        perspective(camera.Fov, aspectRatio, 0.1f, 100.0f, projection);
        
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setFloat("time", totalTime);
        
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        glDepthMask(GL_TRUE);
    }
    
    ~Skybox() {
        if (skyboxVAO) glDeleteVertexArrays(1, &skyboxVAO);
        if (skyboxVBO) glDeleteBuffers(1, &skyboxVBO);
        delete skyboxShader;
    }

private:
    void setupSkyboxGeometry() {
        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
    void createSkyboxShader() {
        std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            out vec3 WorldPos;
            uniform mat4 projection;
            uniform mat4 view;
            void main() {
                WorldPos = aPos;
                vec4 pos = projection * view * vec4(aPos, 1.0);
                gl_Position = pos.xyww;
            }
        )";
        
        std::string fragmentSource = R"(
            #version 330 core
            in vec3 WorldPos;
            out vec4 FragColor;
            uniform float time;
            
            float random(vec2 st) {
                return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
            }
            
            vec3 generateStars(vec3 direction) {
                vec2 uv = direction.xy * 120.0;
                vec3 starColor = vec3(0.0);
                
                vec2 id = floor(uv);
                vec2 gv = fract(uv) - 0.5;
                
                float starRand = random(id);
                if (starRand > 0.88) {
                    float dist = length(gv);
                    float starSize = 0.04 + random(id + 100.0) * 0.02;
                    
                    if (dist < starSize) {
                        float intensity = 1.0 - (dist / starSize);
                        intensity = pow(intensity, 2.0);
                        
                        // Twinkling
                        float twinkle = 0.8 + 0.2 * sin(time * 3.0 + random(id) * 6.28);
                        
                        // Different neon colors
                        vec3 baseColor = vec3(1.0);
                        float colorChoice = random(id + 200.0);
                        if (colorChoice < 0.2) {
                            baseColor = vec3(1.0, 0.4, 0.7);      // Hot pink
                        } else if (colorChoice < 0.4) {
                            baseColor = vec3(0.4, 0.7, 1.0);      // Cyan
                        } else if (colorChoice < 0.6) {
                            baseColor = vec3(0.7, 1.0, 0.4);      // Electric green
                        } else if (colorChoice < 0.8) {
                            baseColor = vec3(1.0, 0.7, 0.3);      // Orange
                        } else {
                            baseColor = vec3(0.8, 0.4, 1.0);      // Purple
                        }
                        
                        starColor = baseColor * intensity * twinkle * 1.5;
                    }
                }
                
                return starColor;
            }
            
            vec3 generateMeteors(vec3 direction) {
                vec3 meteorColor = vec3(0.0);
                
                // 3 meteors at different times
                for (int i = 0; i < 3; i++) {
                    float meteorTime = time * 0.4 + float(i) * 3.0;
                    float meteorPhase = fract(meteorTime / 8.0); // 8 second cycle
                    
                    if (meteorPhase < 0.3) { // Only show for 30% of cycle
                        vec3 meteorStart = normalize(vec3(
                            sin(float(i) * 2.1),
                            0.8 + 0.2 * sin(float(i) * 1.7),
                            cos(float(i) * 2.3)
                        ));
                        
                        vec3 meteorEnd = meteorStart + vec3(-0.8, -1.2, -0.3) * meteorPhase * 3.0;
                        meteorEnd = normalize(meteorEnd);
                        
                        // Check if we're looking at the meteor trail
                        for (float t = 0.0; t < 1.0; t += 0.1) {
                            vec3 meteorPos = mix(meteorStart, meteorEnd, t);
                            float meteorDot = max(0.0, dot(direction, meteorPos));
                            
                            if (meteorDot > 0.99) {
                                float intensity = pow((meteorDot - 0.99) / 0.01, 0.5);
                                float trailFade = 1.0 - t;
                                
                                // Neon meteor colors
                                vec3 meteorCol;
                                if (i == 0) meteorCol = vec3(0.0, 1.0, 1.0);      // Cyan
                                else if (i == 1) meteorCol = vec3(1.0, 0.3, 0.8); // Hot pink
                                else meteorCol = vec3(0.8, 1.0, 0.2);             // Lime green
                                
                                meteorColor += meteorCol * intensity * trailFade * 3.0;
                            }
                        }
                    }
                }
                
                return meteorColor;
            }
            
            void main() {
                vec3 direction = normalize(WorldPos);
                
                // Very dark base sky
                vec3 skyColor = vec3(0.01, 0.01, 0.02);
                
                // Add subtle gradient to horizon
                float horizon = 1.0 - abs(direction.y);
                skyColor += vec3(0.02, 0.01, 0.03) * pow(horizon, 3.0);
                
                // Add stars
                skyColor += generateStars(direction);
                
                // Add meteors
                skyColor += generateMeteors(direction);
                
                // PSX color quantization
                skyColor = floor(skyColor * 32.0) / 32.0;
                
                FragColor = vec4(skyColor, 1.0);
            }
        )";
        
        skyboxShader = new Shader(vertexSource, fragmentSource, true);
    }
    
    void perspective(float fovy, float aspect, float zNear, float zFar, float* result) {
        float f = 1.0f / tan(fovy * 3.14159265359f / 360.0f);
        result[0] = f / aspect; result[4] = 0; result[8] = 0; result[12] = 0;
        result[1] = 0; result[5] = f; result[9] = 0; result[13] = 0;
        result[2] = 0; result[6] = 0; result[10] = (zFar + zNear) / (zNear - zFar); result[14] = (2 * zFar * zNear) / (zNear - zFar);
        result[3] = 0; result[7] = 0; result[11] = -1; result[15] = 0;
    }
};