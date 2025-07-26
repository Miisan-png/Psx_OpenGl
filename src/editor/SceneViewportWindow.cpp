#include "editor/SceneViewportWindow.h"
#include <imgui.h>
#include <glad/glad.h>

SceneViewportWindow::SceneViewportWindow() : isOpen(false) {
    viewportCamera = Camera(0.0f, 5.0f, 10.0f);
    viewportCamera.Yaw = -90.0f;
    viewportCamera.Pitch = -20.0f;
    viewportCamera.updateCameraVectors();
    
    showGrid = true;
    showGizmos = true;
    showWireframe = false;
    gridSize = 50;
    gridSpacing = 1.0f;
    
    lastMouseX = 0.0f;
    lastMouseY = 0.0f;
    isDragging = false;
    isMiddleMouseDown = false;
    
    CreateGridShader();
    CreateGridGeometry();
    CreateFramebuffer(512, 512);
}

void SceneViewportWindow::Draw(Game& game) {
    if (!isOpen) return;
    
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Scene Viewport", &isOpen)) {
        ImGui::End();
        return;
    }
    
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            ImGui::Checkbox("Show Grid", &showGrid);
            ImGui::Checkbox("Show Gizmos", &showGizmos);
            ImGui::Checkbox("Wireframe", &showWireframe);
            ImGui::Separator();
            if (ImGui::Button("Reset Camera")) {
                viewportCamera = Camera(0.0f, 5.0f, 10.0f);
                viewportCamera.Yaw = -90.0f;
                viewportCamera.Pitch = -20.0f;
                viewportCamera.updateCameraVectors();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Grid")) {
            ImGui::SliderInt("Grid Size", &gridSize, 10, 100);
            ImGui::SliderFloat("Grid Spacing", &gridSpacing, 0.5f, 5.0f);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (viewportPanelSize.x != framebufferWidth || viewportPanelSize.y != framebufferHeight) {
        ResizeFramebuffer((int)viewportPanelSize.x, (int)viewportPanelSize.y);
    }
    
    RenderViewport(game);
    
    ImGui::Image((ImTextureID)(intptr_t)colorTexture, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
    
    HandleInput();
    
    ImGui::End();
}

void SceneViewportWindow::HandleInput() {
    if (!ImGui::IsItemHovered()) return;
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    
    float relativeX = mousePos.x - (windowPos.x + contentMin.x);
    float relativeY = mousePos.y - (windowPos.y + contentMin.y);
    
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
        isMiddleMouseDown = true;
        lastMouseX = relativeX;
        lastMouseY = relativeY;
    }
    
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
        isMiddleMouseDown = false;
        isDragging = false;
    }
    
    if (isMiddleMouseDown && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        if (!isDragging) {
            isDragging = true;
            lastMouseX = relativeX;
            lastMouseY = relativeY;
        }
        
        float deltaX = relativeX - lastMouseX;
        float deltaY = relativeY - lastMouseY;
        
        if (io.KeyShift) {
            float panSpeed = 0.01f;
            viewportCamera.Position[0] -= viewportCamera.Right[0] * deltaX * panSpeed;
            viewportCamera.Position[1] += viewportCamera.Up[1] * deltaY * panSpeed;
            viewportCamera.Position[2] -= viewportCamera.Right[2] * deltaX * panSpeed;
        } else {
            float rotSpeed = 0.3f;
            viewportCamera.ProcessMouseMovement(deltaX * rotSpeed, -deltaY * rotSpeed);
        }
        
        lastMouseX = relativeX;
        lastMouseY = relativeY;
    }
    
    if (io.MouseWheel != 0.0f) {
        float zoomSpeed = 0.5f;
        viewportCamera.Position[0] += viewportCamera.Front[0] * io.MouseWheel * zoomSpeed;
        viewportCamera.Position[1] += viewportCamera.Front[1] * io.MouseWheel * zoomSpeed;
        viewportCamera.Position[2] += viewportCamera.Front[2] * io.MouseWheel * zoomSpeed;
    }
}

void SceneViewportWindow::RenderViewport(Game& game) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    if (showWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    float view[16];
    viewportCamera.GetViewMatrix(view);
    
    float projection[16];
    float aspect = (float)framebufferWidth / (float)framebufferHeight;
    perspective(45.0f, aspect, 0.1f, 100.0f, projection);
    
    if (showGrid) {
        RenderGrid(view, projection);
    }
    
    RenderSceneObjects(game, view, projection);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneViewportWindow::RenderGrid(const float* view, const float* projection) {
    if (!gridShader) return;
    
    gridShader->use();
    gridShader->setMat4("view", view);
    gridShader->setMat4("projection", projection);
    gridShader->setFloat("gridSize", (float)gridSize);
    gridShader->setFloat("gridSpacing", gridSpacing);
    gridShader->setVec3("gridColor", 0.5f, 0.5f, 0.5f);
    gridShader->setVec3("axisXColor", 1.0f, 0.3f, 0.3f);
    gridShader->setVec3("axisZColor", 0.3f, 0.3f, 1.0f);
    
    float modelMatrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    gridShader->setMat4("model", modelMatrix);
    
    glBindVertexArray(gridVAO);
    glDrawElements(GL_LINES, gridIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SceneViewportWindow::RenderSceneObjects(Game& game, const float* view, const float* projection) {
    if (!game.renderer.psxShader) return;
    
    game.renderer.psxShader->use();
    game.renderer.psxShader->setMat4("view", view);
    game.renderer.psxShader->setMat4("projection", projection);
    game.renderer.psxShader->setFloat("u_snapResolution", 128.0f);
    
    game.renderer.psxShader->setFloat("fogStart", 50.0f);
    game.renderer.psxShader->setFloat("fogEnd", 100.0f);
    game.renderer.psxShader->setVec3("fogColor", 0.2f, 0.2f, 0.25f);
    
    game.renderer.psxShader->setBool("spotlightEnabled", false);
    game.renderer.psxShader->setVec3("ambientColor", 0.4f, 0.4f, 0.4f);
    game.renderer.psxShader->setFloat("ambientIntensity", 1.0f);
    
    for (const auto& obj : game.scene.objects) {
        game.renderer.psxShader->setBool("useTexture", obj.useTexture);
        
        if (obj.useTexture && obj.texture) {
            obj.texture->Bind(0);
            game.renderer.psxShader->setInt("ourTexture", 0);
        }
        
        float modelMatrix[16];
        obj.transform.GetMatrix(modelMatrix);
        game.renderer.psxShader->setMat4("model", modelMatrix);
        
        if (obj.model) {
            obj.model->Draw();
        }
    }
}

void SceneViewportWindow::CreateGridShader() {
    std::string vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 worldPos;
        
        void main() {
            worldPos = (model * vec4(aPos, 1.0)).xyz;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";
    
    std::string fragmentSource = R"(
        #version 330 core
        in vec3 worldPos;
        out vec4 FragColor;
        
        uniform vec3 gridColor;
        uniform vec3 axisXColor;
        uniform vec3 axisZColor;
        uniform float gridSpacing;
        
        void main() {
            vec3 color = gridColor;
            
            if (abs(worldPos.z) < 0.05) {
                color = axisXColor;
            } else if (abs(worldPos.x) < 0.05) {
                color = axisZColor;
            }
            
            FragColor = vec4(color, 0.8);
        }
    )";
    
    gridShader = new Shader(vertexSource, fragmentSource, true);
}

void SceneViewportWindow::CreateGridGeometry() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    float halfSize = gridSize * gridSpacing * 0.5f;
    
    for (int i = -gridSize/2; i <= gridSize/2; i++) {
        float pos = i * gridSpacing;
        
        vertices.push_back(-halfSize); vertices.push_back(0.0f); vertices.push_back(pos);
        vertices.push_back(halfSize);  vertices.push_back(0.0f); vertices.push_back(pos);
        
        vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(-halfSize);
        vertices.push_back(pos); vertices.push_back(0.0f); vertices.push_back(halfSize);
    }
    
    for (unsigned int i = 0; i < vertices.size() / 3; i++) {
        indices.push_back(i);
    }
    
    gridIndexCount = indices.size();
    
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glGenBuffers(1, &gridEBO);
    
    glBindVertexArray(gridVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void SceneViewportWindow::CreateFramebuffer(int width, int height) {
    framebufferWidth = width;
    framebufferHeight = height;
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneViewportWindow::ResizeFramebuffer(int width, int height) {
    if (width <= 0 || height <= 0) return;
    
    framebufferWidth = width;
    framebufferHeight = height;
    
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void SceneViewportWindow::perspective(float fovy, float aspect, float zNear, float zFar, float* result) {
    float f = 1.0f / tan(fovy * 3.14159265359f / 360.0f);
    result[0] = f / aspect; result[4] = 0; result[8] = 0; result[12] = 0;
    result[1] = 0; result[5] = f; result[9] = 0; result[13] = 0;
    result[2] = 0; result[6] = 0; result[10] = (zFar + zNear) / (zNear - zFar); result[14] = (2 * zFar * zNear) / (zNear - zFar);
    result[3] = 0; result[7] = 0; result[11] = -1; result[15] = 0;
}

SceneViewportWindow::~SceneViewportWindow() {
    if (gridVAO) glDeleteVertexArrays(1, &gridVAO);
    if (gridVBO) glDeleteBuffers(1, &gridVBO);
    if (gridEBO) glDeleteBuffers(1, &gridEBO);
    if (framebuffer) glDeleteFramebuffers(1, &framebuffer);
    if (colorTexture) glDeleteTextures(1, &colorTexture);
    if (depthTexture) glDeleteTextures(1, &depthTexture);
    delete gridShader;
}