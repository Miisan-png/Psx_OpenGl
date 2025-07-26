#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"

const unsigned int SCREEN_WIDTH = 320;
const unsigned int SCREEN_HEIGHT = 240;
const unsigned int WINDOW_SCALE = 3;

Game game;
float lastX = (SCREEN_WIDTH * WINDOW_SCALE) / 2.0f;
float lastY = (SCREEN_HEIGHT * WINDOW_SCALE) / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool uiMode = false;

int currentScreenWidth = SCREEN_WIDTH * WINDOW_SCALE;
int currentScreenHeight = SCREEN_HEIGHT * WINDOW_SCALE;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    game.renderer.SetAspectRatio((float)width / (float)height);
    currentScreenWidth = width;
    currentScreenHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (uiMode) return; // Don't process mouse movement in UI mode
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Use PlayerController for mouse movement
    game.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        uiMode = !uiMode;
        if (uiMode) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "UI Mode: ON (mouse visible)" << std::endl;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true; // Reset mouse to prevent camera jump
            std::cout << "UI Mode: OFF (camera mode)" << std::endl;
        }
        tabPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }
    
    // Only process player input in camera mode
    if (!uiMode) {
        game.ProcessInput(window, deltaTime);
    }
    
    // Fullscreen toggle works in both modes
    static bool f11Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !f11Pressed) {
        static bool fullscreen = false;
        static int windowedWidth = SCREEN_WIDTH * WINDOW_SCALE;
        static int windowedHeight = SCREEN_HEIGHT * WINDOW_SCALE;
        static int windowedX, windowedY;
        
        if (!fullscreen) {
            glfwGetWindowPos(window, &windowedX, &windowedY);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            game.renderer.SetAspectRatio((float)mode->width / (float)mode->height);
            currentScreenWidth = mode->width;
            currentScreenHeight = mode->height;
            fullscreen = true;
        } else {
            glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
            game.renderer.SetAspectRatio((float)windowedWidth / (float)windowedHeight);
            currentScreenWidth = windowedWidth;
            currentScreenHeight = windowedHeight;
            fullscreen = false;
        }
        f11Pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
        f11Pressed = false;
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        SCREEN_WIDTH * WINDOW_SCALE, 
        SCREEN_HEIGHT * WINDOW_SCALE, 
        "PSX Horror Engine - FPS Controller", 
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "PSX Horror Engine with FPS Controller initialized!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Left Shift - Run" << std::endl;
    std::cout << "  H - Toggle head bob" << std::endl;
    std::cout << "  TAB - Toggle UI mode" << std::endl;

    if (!game.Initialize(window)) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        game.Update(deltaTime);
        game.Render(currentScreenWidth, currentScreenHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    game.Shutdown();
    glfwTerminate();
    return 0;
}