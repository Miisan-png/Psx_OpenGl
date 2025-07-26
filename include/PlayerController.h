#pragma once

#include <GLFW/glfw3.h>
#include <cmath>
#include "Camera.h"

enum class MovementState {
    IDLE,
    WALKING,
    RUNNING
};

class PlayerController {
public:
    // Movement settings
    float walkSpeed = 2.5f;
    float runSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    
    // Head bob settings
    bool headBobEnabled = true;
    float headBobFrequency = 10.0f;     // How fast the head bobs
    float headBobAmplitude = 0.08f;     // How much the head bobs vertically
    float headBobLateralAmplitude = 0.04f; // Side-to-side bob
    
    // Camera constraints
    float maxPitchAngle = 89.0f;
    float minPitchAngle = -89.0f;
    
    // Internal state
    MovementState currentState = MovementState::IDLE;
    float headBobTimer = 0.0f;
    float baseHeight = 0.0f; // Original camera height
    bool isRunning = false;
    
    // Movement input state
    bool movingForward = false;
    bool movingBackward = false;
    bool movingLeft = false;
    bool movingRight = false;
    
    PlayerController(Camera* camera) : playerCamera(camera) {
        if (playerCamera) {
            baseHeight = playerCamera->Position[1];
        }
    }
    
    void ProcessKeyboardInput(GLFWwindow* window, float deltaTime) {
        // Reset movement flags
        movingForward = movingBackward = movingLeft = movingRight = false;
        isRunning = false;
        
        // Check for running (Left Shift)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            isRunning = true;
        }
        
        // Movement input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            movingForward = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            movingBackward = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            movingLeft = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            movingRight = true;
        }
        
        // Update movement state
        UpdateMovementState();
        
        // Apply movement
        ApplyMovement(deltaTime);
        
        // Update head bob
        UpdateHeadBob(deltaTime);
    }
    
    void ProcessMouseMovement(float xOffset, float yOffset) {
        if (!playerCamera) return;
        
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;
        
        playerCamera->Yaw += xOffset;
        playerCamera->Pitch += yOffset;
        
        // Constrain pitch
        if (playerCamera->Pitch > maxPitchAngle) {
            playerCamera->Pitch = maxPitchAngle;
        }
        if (playerCamera->Pitch < minPitchAngle) {
            playerCamera->Pitch = minPitchAngle;
        }
        
        playerCamera->updateCameraVectors();
    }
    
    void SetCamera(Camera* camera) {
        playerCamera = camera;
        if (playerCamera) {
            baseHeight = playerCamera->Position[1];
        }
    }
    
    void ResetHeadBob() {
        headBobTimer = 0.0f;
        if (playerCamera) {
            playerCamera->Position[1] = baseHeight;
        }
    }
    
    void SetHeadBobEnabled(bool enabled) {
        headBobEnabled = enabled;
        if (!enabled) {
            ResetHeadBob();
        }
    }
    
private:
    Camera* playerCamera = nullptr;
    
    void UpdateMovementState() {
        bool isMoving = movingForward || movingBackward || movingLeft || movingRight;
        
        if (!isMoving) {
            currentState = MovementState::IDLE;
        } else if (isRunning) {
            currentState = MovementState::RUNNING;
        } else {
            currentState = MovementState::WALKING;
        }
    }
    
    void ApplyMovement(float deltaTime) {
        if (!playerCamera) return;
        
        float currentSpeed = (currentState == MovementState::RUNNING) ? runSpeed : walkSpeed;
        float velocity = currentSpeed * deltaTime;
        
        // Forward/Backward movement
        if (movingForward) {
            playerCamera->Position[0] += playerCamera->Front[0] * velocity;
            playerCamera->Position[1] += playerCamera->Front[1] * velocity;
            playerCamera->Position[2] += playerCamera->Front[2] * velocity;
        }
        if (movingBackward) {
            playerCamera->Position[0] -= playerCamera->Front[0] * velocity;
            playerCamera->Position[1] -= playerCamera->Front[1] * velocity;
            playerCamera->Position[2] -= playerCamera->Front[2] * velocity;
        }
        
        // Left/Right movement (strafe)
        if (movingLeft) {
            playerCamera->Position[0] -= playerCamera->Right[0] * velocity;
            playerCamera->Position[1] -= playerCamera->Right[1] * velocity;
            playerCamera->Position[2] -= playerCamera->Right[2] * velocity;
        }
        if (movingRight) {
            playerCamera->Position[0] += playerCamera->Right[0] * velocity;
            playerCamera->Position[1] += playerCamera->Right[1] * velocity;
            playerCamera->Position[2] += playerCamera->Right[2] * velocity;
        }
        
        // Keep the camera at base height (no flying)
        if (currentState == MovementState::IDLE) {
            playerCamera->Position[1] = baseHeight;
        }
    }
    
    void UpdateHeadBob(float deltaTime) {
        if (!playerCamera || !headBobEnabled) return;
        
        if (currentState == MovementState::IDLE) {
            // Gradually return to base height when idle
            headBobTimer = 0.0f;
            float currentOffset = playerCamera->Position[1] - baseHeight;
            playerCamera->Position[1] -= currentOffset * deltaTime * 5.0f; // Smooth return
            return;
        }
        
        // Calculate head bob based on movement state
        float bobSpeed = headBobFrequency;
        float bobHeight = headBobAmplitude;
        float bobLateral = headBobLateralAmplitude;
        
        if (currentState == MovementState::RUNNING) {
            bobSpeed *= 1.5f;      // Faster bob when running
            bobHeight *= 1.3f;     // Higher bob when running
            bobLateral *= 1.2f;    // More lateral movement when running
        }
        
        // Update timer
        headBobTimer += deltaTime * bobSpeed;
        
        // Calculate vertical bob (sin wave)
        float verticalBob = sin(headBobTimer) * bobHeight;
        
        // Calculate lateral bob (sin wave offset by 90 degrees, slower frequency)
        float lateralBob = sin(headBobTimer * 0.5f) * bobLateral;
        
        // Apply head bob to camera position
        playerCamera->Position[1] = baseHeight + verticalBob;
        
        // Apply subtle lateral movement by adjusting the right vector influence
        // This creates a subtle swaying effect
        float sideInfluence = lateralBob * 0.1f;
        playerCamera->Position[0] += playerCamera->Right[0] * sideInfluence * deltaTime;
        playerCamera->Position[2] += playerCamera->Right[2] * sideInfluence * deltaTime;
    }
};