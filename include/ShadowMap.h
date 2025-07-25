#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"

class ShadowMap {
public:
    unsigned int depthMapFBO;
    unsigned int depthMap;
    unsigned int shadowWidth = 1024;
    unsigned int shadowHeight = 1024;
    
    Shader* shadowShader;
    
    ShadowMap() {
        setupShadowMap();
        createShadowShader();
    }
    
    void BeginShadowPass(const float* lightPos, const float* lightDir) {
        glViewport(0, 0, shadowWidth, shadowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        shadowShader->use();
        
        float lightProjection[16];
        float lightView[16];
        
        perspective(45.0f, 1.0f, 1.0f, 25.0f, lightProjection);
        lookAt(lightPos, lightDir, lightView);
        
        shadowShader->setMat4("lightSpaceMatrix", multiply(lightProjection, lightView));
    }
    
    void EndShadowPass() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void BindShadowMap(unsigned int textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }
    
    ~ShadowMap() {
        glDeleteFramebuffers(1, &depthMapFBO);
        glDeleteTextures(1, &depthMap);
        delete shadowShader;
    }

private:
    void setupShadowMap() {
        glGenFramebuffers(1, &depthMapFBO);
        
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void createShadowShader() {
        std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            
            uniform mat4 lightSpaceMatrix;
            uniform mat4 model;
            
            void main() {
                gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
            }
        )";
        
        std::string fragmentSource = R"(
            #version 330 core
            
            void main() {
                
            }
        )";
        
        shadowShader = new Shader(vertexSource, fragmentSource, true);
    }
    
    void perspective(float fovy, float aspect, float zNear, float zFar, float* result) {
        float f = 1.0f / tan(fovy * 3.14159265359f / 360.0f);
        result[0] = f / aspect; result[4] = 0; result[8] = 0; result[12] = 0;
        result[1] = 0; result[5] = f; result[9] = 0; result[13] = 0;
        result[2] = 0; result[6] = 0; result[10] = (zFar + zNear) / (zNear - zFar); result[14] = (2 * zFar * zNear) / (zNear - zFar);
        result[3] = 0; result[7] = 0; result[11] = -1; result[15] = 0;
    }
    
    void lookAt(const float* eye, const float* dir, float* result) {
        float center[3] = {eye[0] + dir[0], eye[1] + dir[1], eye[2] + dir[2]};
        float up[3] = {0.0f, 1.0f, 0.0f};
        
        float f[3] = {center[0] - eye[0], center[1] - eye[1], center[2] - eye[2]};
        normalize(f);
        
        float s[3];
        cross(f, up, s);
        normalize(s);
        
        float u[3];
        cross(s, f, u);

        result[0] = s[0]; result[4] = s[1]; result[8] = s[2]; result[12] = -s[0]*eye[0] - s[1]*eye[1] - s[2]*eye[2];
        result[1] = u[0]; result[5] = u[1]; result[9] = u[2]; result[13] = -u[0]*eye[0] - u[1]*eye[1] - u[2]*eye[2];
        result[2] = -f[0]; result[6] = -f[1]; result[10] = -f[2]; result[14] = f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2];
        result[3] = 0; result[7] = 0; result[11] = 0; result[15] = 1;
    }
    
    float* multiply(float* a, float* b) {
        static float result[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[i*4 + j] = 0;
                for (int k = 0; k < 4; k++) {
                    result[i*4 + j] += a[i*4 + k] * b[k*4 + j];
                }
            }
        }
        return result;
    }
    
    void normalize(float* vec) {
        float length = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
        vec[0] /= length; vec[1] /= length; vec[2] /= length;
    }
    
    void cross(float* a, float* b, float* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }
};