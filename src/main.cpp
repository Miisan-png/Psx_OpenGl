#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

// Window dimensions
const unsigned int SCREEN_WIDTH = 320;  // PSX-style low resolution
const unsigned int SCREEN_HEIGHT = 240;
const unsigned int WINDOW_SCALE = 3;    // Scale up for modern displays

// PSX-style vertex snapping resolution
const float VERTEX_SNAP_RESOLUTION = 64.0f;

// Callback for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Disable window resizing for consistent PSX feel
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        SCREEN_WIDTH * WINDOW_SCALE, 
        SCREEN_HEIGHT * WINDOW_SCALE, 
        "PSX Horror Engine", 
        NULL, 
        NULL
    );
    
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport to our PSX resolution
    glViewport(0, 0, SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE);
    
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    
    // PSX-style settings
    glDisable(GL_DITHER);  // Sharp, undithered colors
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // For low-res textures
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "PSX Horror Engine initialized!" << std::endl;

    // Create PSX-style shader with vertex snapping
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform float u_snapResolution;
        
        out vec3 vertexColor;
        
        void main() {
            // PSX-style vertex snapping
            vec3 snappedPos = aPos;
            snappedPos.xy = floor(snappedPos.xy * u_snapResolution) / u_snapResolution;
            
            gl_Position = vec4(snappedPos, 1.0);
            vertexColor = aColor;
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        
        void main() {
            // PSX-style color quantization (optional)
            vec3 quantizedColor = floor(vertexColor * 32.0) / 32.0;
            FragColor = vec4(quantizedColor, 1.0);
        }
    )";

    Shader psxShader(vertexShaderSource, fragmentShaderSource, true);

    // Triangle vertices with colors (PSX-style chunky triangle)
    float vertices[] = {
        // positions        // colors
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top - red
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left - green
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // bottom right - blue
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.1f, 0.05f, 0.15f, 1.0f);  // Dark purple/black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our PSX shader
        psxShader.use();
        psxShader.setFloat("u_snapResolution", VERTEX_SNAP_RESOLUTION);

        // Draw triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}