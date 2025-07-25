#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include <vector>

struct FogSettings {
    float start = 2.0f;
    float end = 8.0f;
    float color[3] = {0.05f, 0.02f, 0.08f};
};

struct Transform {
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    
    void GetMatrix(float* result) const {
        result[0] = scale[0]; result[4] = 0; result[8] = 0; result[12] = position[0];
        result[1] = 0; result[5] = scale[1]; result[9] = 0; result[13] = position[1];
        result[2] = 0; result[6] = 0; result[10] = scale[2]; result[14] = position[2];
        result[3] = 0; result[7] = 0; result[11] = 0; result[15] = 1;
    }
};

struct RenderObject {
    Model* model;
    Texture* texture;
    Transform transform;
    bool useTexture = false;
};

class PSXRenderer {
public:
    Shader* psxShader;
    FogSettings fog;
    float vertexSnapResolution = 64.0f;
    
    PSXRenderer() : psxShader(nullptr) {}
    
    bool Initialize() {
        std::string vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;
            layout (location = 2) in vec2 aTexCoord;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            uniform float u_snapResolution;
            uniform float fogStart;
            uniform float fogEnd;
            
            out vec3 vertexColor;
            out vec2 TexCoord;
            out float fogFactor;
            
            void main() {
                vec4 worldPos = model * vec4(aPos, 1.0);
                vec4 viewPos = view * worldPos;
                vec4 clipPos = projection * viewPos;
                
                clipPos.xy = floor(clipPos.xy * u_snapResolution) / u_snapResolution;
                
                float distance = length(viewPos.xyz);
                fogFactor = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);
                
                gl_Position = clipPos;
                vertexColor = aColor;
                TexCoord = aTexCoord;
            }
        )";

        std::string fragmentSource = R"(
            #version 330 core
            in vec3 vertexColor;
            in vec2 TexCoord;
            in float fogFactor;
            out vec4 FragColor;
            
            uniform sampler2D ourTexture;
            uniform bool useTexture;
            uniform vec3 fogColor;
            
            void main() {
                vec4 texColor = texture(ourTexture, TexCoord);
                vec4 finalColor;
                
                if (useTexture) {
                    finalColor = texColor * vec4(vertexColor, 1.0);
                } else {
                    finalColor = vec4(vertexColor, 1.0);
                }
                
                finalColor.rgb = floor(finalColor.rgb * 32.0) / 32.0;
                finalColor.rgb = mix(fogColor, finalColor.rgb, fogFactor);
                
                FragColor = finalColor;
            }
        )";
        
        psxShader = new Shader(vertexSource, fragmentSource, true);
        return true;
    }
    
    void BeginFrame(Camera& camera) {
        glClearColor(fog.color[0], fog.color[1], fog.color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        psxShader->use();
        psxShader->setFloat("u_snapResolution", vertexSnapResolution);
        psxShader->setFloat("fogStart", fog.start);
        psxShader->setFloat("fogEnd", fog.end);
        psxShader->setVec3("fogColor", fog.color[0], fog.color[1], fog.color[2]);
        
        float view[16];
        camera.GetViewMatrix(view);
        psxShader->setMat4("view", view);
        
        float projection[16];
        perspective(camera.Fov, 320.0f / 240.0f, 0.1f, 100.0f, projection);
        psxShader->setMat4("projection", projection);
    }
    
    void RenderObject(const RenderObject& obj) {
        psxShader->setBool("useTexture", obj.useTexture);
        
        if (obj.useTexture && obj.texture) {
            obj.texture->Bind(0);
            psxShader->setInt("ourTexture", 0);
        }
        
        float modelMatrix[16];
        obj.transform.GetMatrix(modelMatrix);
        psxShader->setMat4("model", modelMatrix);
        
        if (obj.model) {
            obj.model->Draw();
        }
    }
    
    void EndFrame() {
        
    }
    
    ~PSXRenderer() {
        delete psxShader;
    }

private:
    void perspective(float fovy, float aspect, float zNear, float zFar, float* result) {
        float f = 1.0f / tan(fovy * 3.14159265359f / 360.0f);
        result[0] = f / aspect; result[4] = 0; result[8] = 0; result[12] = 0;
        result[1] = 0; result[5] = f; result[9] = 0; result[13] = 0;
        result[2] = 0; result[6] = 0; result[10] = (zFar + zNear) / (zNear - zFar); result[14] = (2 * zFar * zNear) / (zNear - zFar);
        result[3] = 0; result[7] = 0; result[11] = -1; result[15] = 0;
    }
};