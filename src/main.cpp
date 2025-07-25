#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Window dimensions
const unsigned int SCREEN_WIDTH = 320;  // PSX-style low resolution
const unsigned int SCREEN_HEIGHT = 240;
const unsigned int WINDOW_SCALE = 3;    // Scale up for modern displays

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

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.1f, 0.05f, 0.15f, 1.0f);  // Dark purple/black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Render game objects here

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}