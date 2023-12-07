#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <assimp/Importer.hpp>
#include <tsa/assimpModel.h>
#include <iostream>
#include "tsa/waterBufferStuff.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

int main() {
    printf("Initializing...");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    //Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //Global settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
    unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

    ew::Shader fireShader("assets/fireShader.vert", "assets/fireShader.frag");

    //Create cube
    ew::Mesh cubeMesh(ew::createCube(1.0f));
    ew::Mesh waterMesh(ew::createPlane(5.0f, 5.0f, 10));
    ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
    ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
    ew::Mesh testMesh(ew::createPlane(1.0, 1.0, 10));

    //Initialize transforms
    ew::Transform cubeTransform;
    ew::Transform waterTransform;
    ew::Transform sphereTransform;
    ew::Transform fireTransform;
    ew::Transform islandTransform;
    ew::Transform testTransform;
    waterTransform.position = ew::Vec3(0, -1.0, 0);
    sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
    fireTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);
    islandTransform.position = ew::Vec3(44, -10, 4);
    testTransform.rotation = ew::Vec3(90, 0, 0);
    testTransform.position = ew::Vec3(0, 0, 0);

    resetCamera(camera,cameraController);

    //Sets blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Load model file
    //tsa::AssimpModel model("assets/models/testIsland.obj");
    tsa::AssimpModel model("assets/models/zeldaIsland/zeldaIsland.obj");

    //Create water buffers
    tsa::WaterBuffers waterBuffers;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;
        prevTime = time;

        //Update camera
        camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        cameraController.Move(window, &camera, deltaTime);

        waterBuffers.bindReflectionFrameBuffer();

        //RENDER
        glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        shader.setInt("_Text_texture_diffuse", 0);
        shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        //Draw shapes
        shader.setMat4("_Model", cubeTransform.getModelMatrix());
        cubeMesh.draw();

        shader.setMat4("_Model", sphereTransform.getModelMatrix());
        sphereMesh.draw();

        shader.setMat4("_Model", islandTransform.getModelMatrix());
        model.Draw(shader);
        glCullFace(GL_FRONT);
        model.Draw(shader);
        glCullFace(GL_BACK);

        //Fire must be last because opacity and blending messes up if something is drawn after
        fireShader.use();
        fireShader.setFloat("_Time", glfwGetTime());
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        fireShader.setInt("_Text_texture_diffuse", 0);
        fireShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        fireShader.setMat4("_Model", fireTransform.getModelMatrix());

        sphereMesh.draw();

        shader.use();
        shader.setMat4("_Model", waterTransform.getModelMatrix());
        waterMesh.draw();

        waterBuffers.unbindCurrFrameBuffers();

        glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setMat4("_ViewProjection", ew::Mat4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
        glBindTexture(GL_TEXTURE_2D, waterBuffers.getReflectionText());
        shader.setInt("_Text_texture_diffuse", 0);
        shader.setMat4("_Model", testTransform.getModelMatrix());
        testMesh.draw();

        //TODO: Render point lights

        //Render UI
        {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
                ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
                ImGui::Checkbox("Orthographic", &camera.orthographic);
                if (camera.orthographic) {
                    ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
                }
                else {
                    ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
                }
                ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
                ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
                if (ImGui::Button("Reset")) {
                    resetCamera(camera, cameraController);
                }
            }

            ImGui::DragFloat3("Fire Scale", &fireTransform.scale.x, 0.1);
            ImGui::DragFloat3("Island Scale", &islandTransform.scale.x, 0.1);
            ImGui::DragFloat3("Island Position", &islandTransform.position.x, 0.1);

            ImGui::ColorEdit3("BG color", &bgColor.x);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }
    waterBuffers.cleanUpTime();
    printf("Shutting down...");
}

void sceneRender(){

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
    camera.position = ew::Vec3(0, 0, 5);
    camera.target = ew::Vec3(0);
    camera.fov = 60.0f;
    camera.orthoHeight = 6.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.orthographic = false;

    cameraController.yaw = 0.0f;
    cameraController.pitch = 0.0f;
}