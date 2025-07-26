#pragma once

#include <glad/glad.h>
#include <iostream>  // ADD THIS LINE
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Lighting.h"
#include "ParticleSystem.h"
#include "PostProcess.h"
#include "ShadowMap.h"
#include "Skybox.h"
#include <vector>

struct FogSettings {
    float start = 2.0f;
    float end = 8.0f;
    float heightStart = -2.0f;
    float heightEnd = 3.0f;
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
    LightingSystem lighting;
    ParticleSystem* particles;
    PostProcessEffect* postProcess;
    ShadowMap* shadowMap;
    float vertexSnapResolution = 64.0f;
    Skybox* skybox;
    
    float currentAspectRatio = 320.0f / 240.0f;
    int renderWidth = 320;
    int renderHeight = 240;
    
    // FIXED CONSTRUCTOR - ADD skybox(nullptr)
    PSXRenderer() : psxShader(nullptr), particles(nullptr), postProcess(nullptr), shadowMap(nullptr), skybox(nullptr) {}
    
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
            uniform float fogHeightStart;
            uniform float fogHeightEnd;
            
            out vec3 vertexColor;
            out vec2 TexCoord;
            out float fogFactor;
            out vec3 FragPos;
            out vec3 WorldPos;
            
            void main() {
                vec4 worldPos = model * vec4(aPos, 1.0);
                vec4 viewPos = view * worldPos;
                vec4 clipPos = projection * viewPos;
                
                clipPos.xy = floor(clipPos.xy * u_snapResolution) / u_snapResolution;
                
                float distance = length(viewPos.xyz);
                float distanceFog = clamp((fogEnd - distance) / (fogEnd - fogStart), 0.0, 1.0);
                
                float height = worldPos.y;
                float heightFog = clamp((height - fogHeightStart) / (fogHeightEnd - fogHeightStart), 0.0, 1.0);
                
                fogFactor = min(distanceFog, heightFog);
                
                gl_Position = clipPos;
                vertexColor = aColor;
                TexCoord = aTexCoord;
                FragPos = viewPos.xyz;
                WorldPos = worldPos.xyz;
            }
        )";

        std::string fragmentSource = R"(
            #version 330 core
            in vec3 vertexColor;
            in vec2 TexCoord;
            in float fogFactor;
            in vec3 FragPos;
            in vec3 WorldPos;
            out vec4 FragColor;
            
            uniform sampler2D ourTexture;
            uniform bool useTexture;
            uniform vec3 fogColor;
            
            uniform bool spotlightEnabled;
            uniform vec3 spotlightPos;
            uniform vec3 spotlightDir;
            uniform vec3 spotlightColor;
            uniform float spotlightIntensity;
            uniform float spotlightRange;
            uniform float spotlightInnerCone;
            uniform float spotlightOuterCone;
            
            uniform vec3 ambientColor;
            uniform float ambientIntensity;
            
            void main() {
                vec4 texColor = texture(ourTexture, TexCoord);
                vec4 baseColor;
                
                if (useTexture) {
                    baseColor = texColor * vec4(vertexColor, 1.0);
                } else {
                    baseColor = vec4(vertexColor, 1.0);
                }
                
                vec3 lighting = ambientColor * ambientIntensity;
                
                if (spotlightEnabled) {
                    vec3 lightDir = normalize(spotlightPos - WorldPos);
                    float distance = length(spotlightPos - WorldPos);
                    
                    if (distance < spotlightRange) {
                        float theta = dot(lightDir, normalize(-spotlightDir));
                        float epsilon = cos(radians(spotlightInnerCone)) - cos(radians(spotlightOuterCone));
                        float intensity = clamp((theta - cos(radians(spotlightOuterCone))) / epsilon, 0.0, 1.0);
                        
                        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
                        
                        lighting += spotlightColor * spotlightIntensity * intensity * attenuation;
                    }
                }
                
                vec4 finalColor = baseColor * vec4(lighting, 1.0);
                finalColor.rgb = floor(finalColor.rgb * 16.0) / 16.0;
                finalColor.rgb = mix(fogColor, finalColor.rgb, fogFactor);
                
                FragColor = finalColor;
            }
        )";

        
        
        psxShader = new Shader(vertexSource, fragmentSource, true);
        particles = new ParticleSystem(2000);
        postProcess = new PostProcessEffect(renderWidth, renderHeight);
        shadowMap = new ShadowMap();

        skybox = new Skybox();
        if (!skybox->Initialize()) {
            std::cout << "Failed to initialize skybox" << std::endl;
            return false;
        }
        return true;
    }
    
    void SetAspectRatio(float aspect) {
        currentAspectRatio = aspect;
    }
    
    void SetScreenSize(int width, int height) {
        if (postProcess) {
            // Keep PSX resolution for rendering, scale for display
        }
    }
    
   void BeginFrame(Camera& camera) {
        postProcess->BeginRender();
        glClearColor(fog.color[0], fog.color[1], fog.color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox->Render(camera, currentAspectRatio);
        
        lighting.SetFlashlightFromCamera(camera.Position, camera.Front);
        
        psxShader->use();
        psxShader->setFloat("u_snapResolution", vertexSnapResolution);
        psxShader->setFloat("fogStart", fog.start);
        psxShader->setFloat("fogEnd", fog.end);
        psxShader->setFloat("fogHeightStart", fog.heightStart);
        psxShader->setFloat("fogHeightEnd", fog.heightEnd);
        psxShader->setVec3("fogColor", fog.color[0], fog.color[1], fog.color[2]);
        
        psxShader->setBool("spotlightEnabled", lighting.spotlight.enabled);
        psxShader->setVec3("spotlightPos", lighting.spotlight.position[0], lighting.spotlight.position[1], lighting.spotlight.position[2]);
        psxShader->setVec3("spotlightDir", lighting.spotlight.direction[0], lighting.spotlight.direction[1], lighting.spotlight.direction[2]);
        psxShader->setVec3("spotlightColor", lighting.spotlight.color[0], lighting.spotlight.color[1], lighting.spotlight.color[2]);
        psxShader->setFloat("spotlightIntensity", lighting.spotlight.intensity);
        psxShader->setFloat("spotlightRange", lighting.spotlight.range);
        psxShader->setFloat("spotlightInnerCone", lighting.spotlight.innerCone);
        psxShader->setFloat("spotlightOuterCone", lighting.spotlight.outerCone);
        
        psxShader->setVec3("ambientColor", lighting.ambient.color[0], lighting.ambient.color[1], lighting.ambient.color[2]);
        psxShader->setFloat("ambientIntensity", lighting.ambient.intensity);
        
        float view[16];
        camera.GetViewMatrix(view);
        psxShader->setMat4("view", view);
        
        float projection[16];
        perspective(camera.Fov, currentAspectRatio, 0.1f, 100.0f, projection);
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
    
   void EndFrame(Camera& camera, int screenWidth, int screenHeight) {
        float view[16], projection[16];
        camera.GetViewMatrix(view);
        perspective(camera.Fov, currentAspectRatio, 0.1f, 100.0f, projection);
        
        particles->Render(view, projection, camera.Position);
        
        postProcess->EndRender();
        postProcess->RenderToScreen(screenWidth, screenHeight);
    }
    
    void Update(float deltaTime, Camera& camera) {
        skybox->Update(deltaTime);
        particles->Update(deltaTime, camera.Position);
    }
    
    // FIXED DESTRUCTOR - ADD shadowMap deletion
    ~PSXRenderer() {
        delete psxShader;
        delete particles;
        delete postProcess;
        delete shadowMap;  // ADD THIS LINE
        delete skybox;
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