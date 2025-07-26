#pragma once

#include <glad/glad.h>
#include <cmath>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera {
public:
    float Position[3];
    float Front[3];
    float Up[3];
    float Right[3];
    float WorldUp[3];
    
    float Yaw;
    float Pitch;
    
    float MovementSpeed;
    float MouseSensitivity;
    float Fov;

    Camera(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f, 
           float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f, 
           float yaw = YAW, float pitch = PITCH) {
        Position[0] = posX; Position[1] = posY; Position[2] = posZ;
        WorldUp[0] = upX; WorldUp[1] = upY; WorldUp[2] = upZ;
        Yaw = yaw;
        Pitch = pitch;
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;
        Fov = FOV;
        updateCameraVectors();
    }

    void GetViewMatrix(float* matrix) {
        float center[3] = {
            Position[0] + Front[0],
            Position[1] + Front[1], 
            Position[2] + Front[2]
        };
        lookAt(Position, center, Up, matrix);
    }

    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == 0) {
            Position[0] += Front[0] * velocity;
            Position[1] += Front[1] * velocity;
            Position[2] += Front[2] * velocity;
        }
        if (direction == 1) {
            Position[0] -= Front[0] * velocity;
            Position[1] -= Front[1] * velocity;
            Position[2] -= Front[2] * velocity;
        }
        if (direction == 2) {
            Position[0] -= Right[0] * velocity;
            Position[1] -= Right[1] * velocity;
            Position[2] -= Right[2] * velocity;
        }
        if (direction == 3) {
            Position[0] += Right[0] * velocity;
            Position[1] += Right[1] * velocity;
            Position[2] += Right[2] * velocity;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void updateCameraVectors() {
        float front[3];
        front[0] = cos(radians(Yaw)) * cos(radians(Pitch));
        front[1] = sin(radians(Pitch));
        front[2] = sin(radians(Yaw)) * cos(radians(Pitch));
        normalize(front, Front);
        
        cross(Front, WorldUp, Right);
        normalize(Right, Right);
        cross(Right, Front, Up);
        normalize(Up, Up);
    }

private:
    float radians(float degrees) {
        return degrees * 3.14159265359f / 180.0f;
    }

    void normalize(float* vec, float* result) {
        float length = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
        result[0] = vec[0] / length;
        result[1] = vec[1] / length;
        result[2] = vec[2] / length;
    }

    void cross(float* a, float* b, float* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    void lookAt(float* eye, float* center, float* up, float* result) {
        float f[3] = {center[0] - eye[0], center[1] - eye[1], center[2] - eye[2]};
        normalize(f, f);
        
        float s[3];
        cross(f, up, s);
        normalize(s, s);
        
        float u[3];
        cross(s, f, u);

        result[0] = s[0]; result[4] = s[1]; result[8] = s[2]; result[12] = -s[0]*eye[0] - s[1]*eye[1] - s[2]*eye[2];
        result[1] = u[0]; result[5] = u[1]; result[9] = u[2]; result[13] = -u[0]*eye[0] - u[1]*eye[1] - u[2]*eye[2];
        result[2] = -f[0]; result[6] = -f[1]; result[10] = -f[2]; result[14] = f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2];
        result[3] = 0; result[7] = 0; result[11] = 0; result[15] = 1;
    }
};