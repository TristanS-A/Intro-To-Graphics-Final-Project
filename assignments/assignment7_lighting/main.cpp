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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

#define MAX_LIGHTS 5

int currMaxLights = 5;

bool blinnPhong = true;
bool gauraudShading = false;
bool stylizedShading = false;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
    ew::Vec3 position;
    ew::Vec3 color;
};

struct Material {
    float ambientK;
    float diffuseK;
    float shininess;
    float specular;
};

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
	ew::Shader gouraudShader("assets/gouraudLit.vert", "assets/gouraudLit.frag");
	ew::Shader stylishShader("assets/defaultLit.vert", "assets/stylishLit.frag");
    ew::Shader currLitShader = shader;
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

    ew::Shader lightShader("assets/unlit.vert", "assets/unlit.frag");

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);

    //Create materials for meshes
    Material cubeMat = {0, 1, 50, 1};
    Material planeMat = {0, 1, 50, 1};
    Material sphereMat = {0, 1, 50, 1};
    Material cylinderMat = {0, 1, 50, 1};

    //Create lights
    Light lights[MAX_LIGHTS];
	ew::Transform lightTransforms[MAX_LIGHTS];

    lights[0] = {ew::Vec3(2.0f, 1.0f, 2.0f), ew::Vec3(0.5, 0.0, 1.0)};
    lights[1] = {ew::Vec3(-2.0f, 1.0f, 2.0f), ew::Vec3(0.0, 1.0, 0.0)};
    lights[2] = {ew::Vec3(2.0f, 1.0f, -2.0f), ew::Vec3(1.0, 0.0, 0.0)};
    lights[3] = {ew::Vec3(-2.0f, 1.0f, -2.0f), ew::Vec3(0.0, 0.0, 1.0)};
    lights[4] = {ew::Vec3(0.0f, 2.0f, 0.0f), ew::Vec3(0.0, 1.0, 1.0)};

    for (int i = 0; i < MAX_LIGHTS; i++){
        lightTransforms[i].position = lights[i].position;
        lightTransforms[i].scale = ew::Vec3(0.5, 0.5, 0.5);
    }

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currLitShader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		currLitShader.setInt("_Texture", 0);
		currLitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        currLitShader.setVec3("_CamPos", camera.position);
        currLitShader.setInt("_BlinnPhong", blinnPhong);

		//Draw shapes
		currLitShader.setMat4("_Model", cubeTransform.getModelMatrix());
		currLitShader.setFloat("_Mat.ambientK", cubeMat.ambientK);
        currLitShader.setFloat("_Mat.diffuseK", cubeMat.diffuseK);
        currLitShader.setFloat("_Mat.shininess", cubeMat.shininess);
        currLitShader.setFloat("_Mat.specular", cubeMat.specular);
		cubeMesh.draw();

		currLitShader.setMat4("_Model", planeTransform.getModelMatrix());
        currLitShader.setFloat("_Mat.ambientK", planeMat.ambientK);
        currLitShader.setFloat("_Mat.diffuseK", planeMat.diffuseK);
        currLitShader.setFloat("_Mat.shininess", planeMat.shininess);
        currLitShader.setFloat("_Mat.specular", planeMat.specular);
		planeMesh.draw();

		currLitShader.setMat4("_Model", sphereTransform.getModelMatrix());
        currLitShader.setFloat("_Mat.ambientK", sphereMat.ambientK);
        currLitShader.setFloat("_Mat.diffuseK", sphereMat.diffuseK);
        currLitShader.setFloat("_Mat.shininess", sphereMat.shininess);
        currLitShader.setFloat("_Mat.specular", sphereMat.specular);
		sphereMesh.draw();

		currLitShader.setMat4("_Model", cylinderTransform.getModelMatrix());
        currLitShader.setFloat("_Mat.ambientK", cylinderMat.ambientK);
        currLitShader.setFloat("_Mat.diffuseK", cylinderMat.diffuseK);
        currLitShader.setFloat("_Mat.shininess", cylinderMat.shininess);
        currLitShader.setFloat("_Mat.specular", cylinderMat.specular);
		cylinderMesh.draw();

        //Drawing Lights
		currLitShader.setInt("_CurrMaxLights", currMaxLights);

        //Assign light data to lit meshes
        for (int i = 0; i < currMaxLights; i++){
            lights[i].position = lightTransforms[i].position;
            currLitShader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
            currLitShader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
        }

        lightShader.use();
        lightShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        //Assign data to light meshes
        for (int i = 0; i < currMaxLights; i++) {
            lightShader.setMat4("_Model", lightTransforms[i].getModelMatrix());
            lightShader.setVec3("_Color", lights[i].color);
            sphereMesh.draw();
        }

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

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::DragInt("Number of Lights", &currMaxLights, 1, 0, MAX_LIGHTS);
            ImGui::Checkbox("Blinn-Phong", &blinnPhong);
            if (ImGui::Checkbox("Gouraud Shading", &gauraudShading)){
                if (gauraudShading){
                    stylizedShading = false;
                    currLitShader = gouraudShader;
                }
                else{
                    if (stylizedShading){
                        currLitShader = stylishShader;
                    }
                    else{
                        currLitShader = shader;
                    }
                }
            }
            if (ImGui::Checkbox("Epic Stylized Shading", &stylizedShading)){
                if (stylizedShading){
                    gauraudShading = false;
                    currLitShader = stylishShader;
                }
                else{
                    if (gauraudShading){
                        currLitShader = gouraudShader;
                    }
                    else{
                        currLitShader = shader;
                    }
                }
            }
            if (ImGui::CollapsingHeader("Lights")) {
                for (int i = 0; i < currMaxLights; i++) {
                    if (ImGui::CollapsingHeader(("Light " + std::to_string(i + 1)).c_str())) {
                        ImGui::DragFloat3("Light Position", &lightTransforms[i].position.x, 0.1f);
                        ImGui::ColorEdit3("Light Color", &lights[i].color.x);
                    }
                }
            }
            if (ImGui::CollapsingHeader("Mesh Materials")) {
                if (ImGui::CollapsingHeader("Cube Material")) {
                    ImGui::DragFloat("Diffuse K", &cubeMat.diffuseK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Ambient K", &cubeMat.ambientK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Shininess K", &cubeMat.shininess, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Specular K", &cubeMat.specular, 0.1f, 0.0, 1000);
                }
                if (ImGui::CollapsingHeader("Cylinder Material")) {
                    ImGui::DragFloat("Diffuse K", &cylinderMat.diffuseK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Ambient K", &cylinderMat.ambientK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Shininess K", &cylinderMat.shininess, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Specular K", &cylinderMat.specular, 0.1f, 0.0, 1000);
                }
                if (ImGui::CollapsingHeader("Sphere Material")) {
                    ImGui::DragFloat("Diffuse K", &sphereMat.diffuseK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Ambient K", &sphereMat.ambientK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Shininess K", &sphereMat.shininess, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Specular K", &sphereMat.specular, 0.1f, 0.0, 1000);
                }
                if (ImGui::CollapsingHeader("Plane Material")) {
                    ImGui::DragFloat("Diffuse K", &planeMat.diffuseK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Ambient K", &planeMat.ambientK, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Shininess K", &planeMat.shininess, 0.1f, 0.0, 1000);
                    ImGui::DragFloat("Specular K", &planeMat.specular, 0.1f, 0.0, 1000);
                }
            }
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
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


