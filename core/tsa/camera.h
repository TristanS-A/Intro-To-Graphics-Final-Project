//
// Created by tropi on 10/11/2023.
//

#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <ew/ewMath/ewMath.h>
#include "transfromations.h"

namespace tsa{
    struct Camera {
        ew::Vec3 position;
        ew::Vec3 target;
        float fov;
        float aspect;
        float nearPlain;
        float farPlain;
        bool orthographic;
        float orthoSize;
        ew::Mat4 ViewMatrix(){
            return tsa::LookAt(position, target, ew::Vec3(0, 1, 0));
        };
        ew::Mat4 ProjectionMatrix(){
            if (orthographic){
                return Orthographic(orthoSize, aspect, nearPlain, farPlain);
            }
            else {
                return Perspective(ew::Radians(fov), aspect, nearPlain, farPlain);
            }
        };
    };

    struct CameraControls {
        double prevMouseX, prevMouseY;
        float yaw = 0, pitch = 0;
        float mouseSensitivity = 0.1f;
        bool firstMouse = true;
        float moveSpeed = 5.0f;

        void moveCamera(GLFWwindow *window, Camera *camera, float deltaTime){
            if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)){
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouse = true;
                return;
            }

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);    

            if (firstMouse){
                firstMouse = false;
                prevMouseX = mouseX;
                prevMouseY = mouseY;
            }

            const float clampLow = -89;
            const float clampHigh = 89;
            pitch += mouseSensitivity * (mouseY - prevMouseY);
            yaw -= mouseSensitivity * (mouseX - prevMouseX);

            pitch = tsa::clamp(pitch, clampLow, clampHigh);

            float newPitch = ew::Radians(pitch);
            float newYaw = ew::Radians(yaw);

            ew::Vec3 forward = ew::Vec3(sin(newYaw) * cos(newPitch), sin(newPitch), -cos(newYaw) * cos(newPitch));

            prevMouseX = mouseX;
            prevMouseY = mouseY;

            ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0, 1, 0)));
            ew::Vec3 up = ew::Normalize(ew::Cross(forward, right));

            if (glfwGetKey(window,GLFW_KEY_W)){
                camera->position += forward * moveSpeed * deltaTime;
            }
            else if (glfwGetKey(window,GLFW_KEY_S)){
                camera->position += forward * -moveSpeed * deltaTime;
            }

            if (glfwGetKey(window,GLFW_KEY_A)){
                camera->position += right * -moveSpeed * deltaTime;
            }
            else if (glfwGetKey(window,GLFW_KEY_D)){
                camera->position += right * moveSpeed * deltaTime;
            }

            if (glfwGetKey(window,GLFW_KEY_Q)){
                camera->position += up * moveSpeed * deltaTime;
            }
            else if (glfwGetKey(window,GLFW_KEY_E)){
                camera->position += up * -moveSpeed * deltaTime;
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
                moveSpeed = 10.0;
            }
            else if (moveSpeed != 5.0){
                moveSpeed = 5.0;
            }

            camera->target = camera->position + forward;
        }
    };
}