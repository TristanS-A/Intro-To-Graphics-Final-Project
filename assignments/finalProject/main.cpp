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
#include <tsa/procGen.h>
#include "tsa/waterBufferStuff.h"

#define MAX_LIGHTS 1

struct Light {
    ew::Vec3 position;
    ew::Vec3 color;
    float range;
};

struct Material {
    float ambientK;
    float diffuseK;
    float shininess;
    float specular;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.0, 0.0, 0.0);

ew::Camera camera;
ew::CameraController cameraController;

void sceneRender(ew::Shader shader, unsigned int brickTexture, ew::Transform islandTransform, tsa::AssimpModel model, ew::Transform waterTransform, Light lights[], ew::Transform lightTransforms[], Material islandMat){
    glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    //Assign Material data to lit shaders
    shader.setFloat("_Mat.ambientK", islandMat.ambientK);
    shader.setFloat("_Mat.diffuseK", islandMat.diffuseK);
    shader.setFloat("_Mat.shininess", islandMat.shininess);
    shader.setFloat("_Mat.specular", islandMat.specular);

    //Assign light data to lit shaders
    for (int i = 0; i < MAX_LIGHTS; i++){
        lights[i].position = lightTransforms[i].position;
        shader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
        shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
        shader.setFloat("_Lights[" + std::to_string(i) + "].range", lights[i].range);
    }

    shader.setVec3("_CamPos", camera.position);

    shader.setVec4("_ClipPlane", ew::Vec4(0, 1, 0, waterTransform.position.y));
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    shader.setInt("_Text_texture_diffuse", 0);
    shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

    shader.setMat4("_Model", islandTransform.getModelMatrix());
    model.Draw(shader);
    glCullFace(GL_FRONT);
    model.Draw(shader);
    glCullFace(GL_BACK);
}

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
    unsigned int waterNormalText = ew::loadTexture("assets/waterNormalMap.png",GL_REPEAT,GL_LINEAR);

    ew::Shader fireShader("assets/fireShader.vert", "assets/fireShader.frag");
    ew::Shader waterShader("assets/waterShader.vert", "assets/waterShader.frag");
    ew::Shader skyShader("assets/skyShader.vert", "assets/skyShader.frag");

    //Create cube
    ew::Mesh waterMesh(ew::createPlane(5.0f, 5.0f, 50));
    ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));

    //Create sky"box"
    float rad = 50.0f;
    int segments = 64;
    float height = 25.0f;
    ew::Mesh skyTop(tsa::createDomeTop(rad, segments));
    ew::Mesh skyBot(tsa::createDomeBot(height, rad, segments));

    //Initialize transforms
    ew::Transform cubeTransform;
    ew::Transform waterTransform;
    ew::Transform sphereTransform;
    ew::Transform fireTransform;
    ew::Transform islandTransform;
    ew::Transform testTransform;
    ew::Transform skyTransform;
    ew::Transform seaTransform;
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
    tsa::AssimpModel model("assets/models/testIsland/testIsland.obj");

    //Create water buffers
    tsa::WaterBuffers waterBuffers;

    //Create lights
    Light lights[MAX_LIGHTS];
    ew::Transform lightTransforms[MAX_LIGHTS];

    lights[0] = {ew::Vec3(2.0f, 1.0f, 2.0f), ew::Vec3(0.6, 0.5, 0.0), 5};

    for (int i = 0; i < MAX_LIGHTS; i++){
        lightTransforms[i].position = lights[i].position;
        lightTransforms[i].scale = ew::Vec3(0.5, 0.5, 0.5);
    }

    Material islandMat = {0.0, 1.0, 500, 1.0};

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

        float reflectionCamOffset = 2 * (camera.position.y - waterTransform.position.y);

        camera.position.y -= reflectionCamOffset;
        cameraController.pitch *= -1;

        glEnable(GL_CLIP_DISTANCE0);
        sceneRender(shader, brickTexture, islandTransform, model, waterTransform, lights, lightTransforms, islandMat);

        //Assign data to light meshes
        fireShader.use();
        fireShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        fireShader.setFloat("_Time", glfwGetTime());
        for (int i = 0; i < MAX_LIGHTS; i++) {
            fireShader.setMat4("_Model", lightTransforms[i].getModelMatrix());
            fireShader.setVec3("_Color", lights[i].color);
            sphereMesh.draw();
        }

        camera.position.y += reflectionCamOffset;
        cameraController.pitch *= -1;

        waterBuffers.unbindWaterFrameBuffers(SCREEN_WIDTH, SCREEN_HEIGHT);

        glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CLIP_DISTANCE0);
        sceneRender(shader, brickTexture, islandTransform, model, waterTransform, lights, lightTransforms, islandMat);

        waterShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterBuffers.getReflectionText());
        waterShader.setInt("_ReflectionTexture", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, waterNormalText);
        waterShader.setInt("_NormalMap", 1);
        waterShader.setFloat("_Time", glfwGetTime());
        waterShader.setVec3("_CamPos", camera.position);
        waterShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        waterShader.setMat4("_Model", waterTransform.getModelMatrix());

        //Assign light data to water shaders
        for (int i = 0; i < MAX_LIGHTS; i++){
            lights[i].position = lightTransforms[i].position;
            waterShader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
            waterShader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
            waterShader.setFloat("_Lights[" + std::to_string(i) + "].range", lights[i].range);
        }

        waterMesh.draw();


        //Assign data to light meshes
        fireShader.use();
        fireShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        fireShader.setFloat("_Time", glfwGetTime());

        //Render lights
        for (int i = 0; i < MAX_LIGHTS; i++) {
            fireShader.setMat4("_Model", lightTransforms[i].getModelMatrix());
            fireShader.setVec3("_Color", lights[i].color);
            sphereMesh.draw();
        }

        skyTransform.position = ew::Vec3(0.0, 0.0, 0.0);
        seaTransform.position = ew::Vec3(0.0, 0.0, 0.0);

        skyShader.use();
        skyShader.setMat4("_View", camera.ViewMatrix());
        skyShader.setMat4("_Model", skyTransform.getModelMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        skyShader.setInt("_Texture", 0); //placeholder, replace with actual texture later
        skyTop.draw();
        skyShader.setMat4("_Model", seaTransform.getModelMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        skyShader.setInt("_Texture", 0); //placeholder, replace with actual texture later
        skyBot.draw();



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

            if (ImGui::CollapsingHeader("Lights")) {
                for (int i = 0; i < MAX_LIGHTS; i++) {
                    if (ImGui::CollapsingHeader(("Light " + std::to_string(i + 1)).c_str())) {
                        ImGui::DragFloat3("Light Position", &lightTransforms[i].position.x, 0.1f);
                        ImGui::ColorEdit3("Light Color", &lights[i].color.x);
                        ImGui::DragFloat("Light Range", &lights[i].range);
                    }
                }
            }

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