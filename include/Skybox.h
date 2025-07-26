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
        
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        
        skyboxShader->use();
        
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
        
        glDepthFunc(GL_LESS);
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
            
            float hash(float n) {
                return fract(sin(n) * 43758.5453);
            }
            
            float hash2(vec2 p) {
                return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
            }
            
            vec3 generateStars(vec3 direction) {
                vec3 starColor = vec3(0.0);
                vec3 absDir = abs(direction);
                vec2 uv = vec2(0.0);
                
                if (absDir.x >= absDir.y && absDir.x >= absDir.z) {
                    uv = direction.yz / absDir.x;
                } else if (absDir.y >= absDir.x && absDir.y >= absDir.z) {
                    uv = direction.xz / absDir.y;
                } else {
                    uv = direction.xy / absDir.z;
                }
                
                uv = uv * 0.5 + 0.5;
                uv *= 20.0;
                
                vec2 grid = floor(uv);
                vec2 frac = fract(uv);
                
                float starRand = hash2(grid);
                if (starRand > 0.92) {
                    vec2 starPos = vec2(hash2(grid + 100.0), hash2(grid + 200.0));
                    float dist = distance(frac, starPos);
                    
                    if (dist < 0.1) {
                        float intensity = 1.0 - (dist / 0.1);
                        intensity = intensity * intensity;
                        
                        float twinkle = 0.7 + 0.3 * sin(time * 2.0 + hash2(grid) * 6.28);
                        
                        vec3 baseColor = vec3(1.0);
                        float colorChoice = hash2(grid + 300.0);
                        if (colorChoice < 0.15) {
                            baseColor = vec3(1.0, 0.3, 0.8);
                        } else if (colorChoice < 0.3) {
                            baseColor = vec3(0.3, 0.8, 1.0);
                        } else if (colorChoice < 0.45) {
                            baseColor = vec3(0.8, 1.0, 0.3);
                        } else if (colorChoice < 0.6) {
                            baseColor = vec3(1.0, 0.8, 0.3);
                        } else if (colorChoice < 0.75) {
                            baseColor = vec3(0.8, 0.3, 1.0);
                        } else if (colorChoice < 0.9) {
                            baseColor = vec3(1.0, 0.5, 0.5);
                        }
                        
                        starColor = baseColor * intensity * twinkle * 2.0;
                    }
                }
                
                return starColor;
            }
            

            
            void main() {
                vec3 direction = normalize(WorldPos);
                
                vec3 skyColor = vec3(0.008, 0.008, 0.015);
                
                float horizon = 1.0 - abs(direction.y);
                skyColor += vec3(0.015, 0.008, 0.025) * pow(horizon, 4.0);
                
                skyColor += generateStars(direction);
                
                skyColor = floor(skyColor * 16.0) / 16.0;
                
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