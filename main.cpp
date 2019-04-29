#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drop_callback(GLFWwindow* window, int count, const char** paths);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods); // 响应鼠标点击事件
void handleLightType(Shader &multipleLightingShader, Shader &mutipleLightLampShader, int lightType); // 改变光的类型
void dynamicRenderLights(int number, Shader lightingShader); // 动态渲染光源
void renderMaterial(int materialIndex, Shader lightingShader, Shader lampShader, int materialActive, unsigned int lightVAO); // 动态改变材质
void loadModelFromFile(string filePath, vector<float> &VectorofVertice, vector<unsigned int> &VectorofFace); // 从文件加载模型
void renderModel(vector<float> VectorofVertice, vector<unsigned int> VectorofFace, unsigned &modelVBO, unsigned &modelVAO, unsigned &modelEBO); // 设置模型的VAO、VBO、EBO
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char *path);


// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
//string filePath = "C:\\Users\\xiong\\source\\repos\\Project6\\Project6\\bunny.poly"; // 测试使用
string filePath = "";
unsigned int numOfLights = 5;
unsigned int materialIndex = 1;
int materialActive = 0;
int lightType = 0;
bool lightRotate = false;
glm::mat4 transform = glm::mat4(1.0f);
int needChange = 1;
bool enableTransShader = false;

vector<float> VectorofVertice;
vector<unsigned int> VectorofFace;
unsigned modelVBO, modelVAO, modelEBO;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// default light position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bing & JiaYan Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// glfw的callback全部放在这里
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); //最后再打开
	glfwSetKeyCallback(window, key_callback);
	glfwSetDropCallback(window, drop_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	// 多光源的shader
	Shader multipleLightingShader("multiplelightsvs.txt", "multiplelightsfs.txt");
	Shader mutipleLightLampShader("multiplelightslampvs.txt", "multiplelightslampfs.txt");
	// 材质的shader
	Shader lightingShader("materialsvs.txt", "materialsfs.txt");
	Shader lampShader("materialslampvs.txt", "materialslampfs.txt");
	// 基础光照的shader
	Shader basicLightingShader("basiclightingvs.txt", "basiclightingfs.txt");
	Shader basicLampShader("basiclightinglampvs.txt", "basiclightinglampfs.txt");
	// transformation的shader
	Shader transShader("transformvs.txt", "transformfs.txt");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float verticesCube[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// 如果没有默认的模型就通过拖拽文件来加载
	if (filePath != "") {
		loadModelFromFile(filePath, VectorofVertice, VectorofFace);
		renderModel(VectorofVertice, VectorofFace, modelVBO, modelVAO, modelEBO);
	}

	// positions all containers
	glm::vec3 modelPositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
	};

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.0f, 1.0f,  1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  -1.0f, 1.0f)
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	//unsigned int diffuseMap = loadTexture("container2.png");
	//unsigned int specularMap = loadTexture("container2_specular.png");

	// shader configuration
	// --------------------
	multipleLightingShader.use();
	multipleLightingShader.setInt("material.diffuse", 0);
	multipleLightingShader.setInt("material.specular", 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 动态光源
		lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
		lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
		lightPos.z = sin(glfwGetTime() / 3.0f) * 0.5f;

		renderMaterial(materialIndex, lightingShader, lampShader, materialActive, lightVAO); // 渲染材质

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		multipleLightingShader.setMat4("projection", projection);
		multipleLightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		multipleLightingShader.setMat4("model", model);

		// transformation shader
		if (enableTransShader) {
			transShader.use();
			unsigned int transformLoc = glGetUniformLocation(transShader.ID, "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		}

		// 把模型渲染出来	
		glBindVertexArray(modelVAO);
		glDrawElements(GL_TRIANGLES, int(VectorofFace.size()), GL_UNSIGNED_INT, 0);

		// also draw the lamp object
		if (lightRotate) {
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			lampShader.setMat4("model", model);

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	
		// 多光源渲染
		// 注意这里只是显示出来并不代表真正的光源
		if (lightType == 1) {
			mutipleLightLampShader.use();
			mutipleLightLampShader.setMat4("projection", projection);
			mutipleLightLampShader.setMat4("view", view);
			glBindVertexArray(lightVAO);
			for (unsigned int i = 0; i < numOfLights; i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				mutipleLightLampShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		handleLightType(multipleLightingShader, mutipleLightLampShader, lightType);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		materialIndex = 1;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		materialIndex = 2;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		materialIndex = 3;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		materialIndex = 4;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		Sleep(300);
		if (materialActive == 1) {
			materialActive = 0;
		}
		else {
			materialActive = 1;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		lightType = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		Sleep(300);
		lightRotate = !lightRotate;
	}
	// set all shader to default
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Sleep(300);
		materialActive = 0;
		lightType = 0;
		glUseProgram(0);
	}
	// if press Q
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0, 0.0, 1.0));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, 0.1f, 0.0f));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, -0.1f, 0.0f));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.1f, 0.0f, 0.0f));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(-0.1f, 0.0f, 0.0f));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}
	// camera
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(-0.1f, 0.0f, 0.0f));
		unsigned int transformLoc = glGetUniformLocation(6, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		Sleep(100);
	}

	// disable transformation shader
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		Sleep(500);
		cout << "pressed N" << endl;;
		if (enableTransShader) {
			transform = glm::mat4(1.0f);
			enableTransShader = false;
		}
		else {
			enableTransShader = true;
		}
		cout << "enableTransShader:" << enableTransShader << endl;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//if (key == GLFW_KEY_E && action == GLFW_PRESS) {
	//	cout << "pressed E+I" << endl;
	//	transform = glm::rotate(transform, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
	//	unsigned int transformLoc = glGetUniformLocation(6, "transform");
	//	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	//	Sleep(500);
	//}
	//if (key == GLFW_KEY_O && action == GLFW_PRESS) {
	//	cout << "pressed O" << endl;
	//}
}


void handleLightType(Shader &multipleLightingShader, Shader &mutipleLightLampShader, int lightType) {
	//cout << lightType << endl;
	if (lightType == 0) {
		// do nothing
	}
	if (lightType == 1) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f,  -1.0f, 1.0f)
		};
		// be sure to activate shader when setting uniforms/drawing objects
		multipleLightingShader.use();
		multipleLightingShader.setVec3("viewPos", camera.Position);
		multipleLightingShader.setFloat("material.shininess", 32.0f);

		// directional light
		multipleLightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		multipleLightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		multipleLightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		multipleLightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		multipleLightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		multipleLightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		multipleLightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		multipleLightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setFloat("pointLights[0].constant", 1.0f);
		multipleLightingShader.setFloat("pointLights[0].linear", 0.09);
		multipleLightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		multipleLightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		multipleLightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		multipleLightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		multipleLightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setFloat("pointLights[1].constant", 1.0f);
		multipleLightingShader.setFloat("pointLights[1].linear", 0.09);
		multipleLightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		multipleLightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		multipleLightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		multipleLightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		multipleLightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setFloat("pointLights[2].constant", 1.0f);
		multipleLightingShader.setFloat("pointLights[2].linear", 0.09);
		multipleLightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		multipleLightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		multipleLightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		multipleLightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		multipleLightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setFloat("pointLights[3].constant", 1.0f);
		multipleLightingShader.setFloat("pointLights[3].linear", 0.09);
		multipleLightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		multipleLightingShader.setVec3("spotLight.position", camera.Position);
		multipleLightingShader.setVec3("spotLight.direction", camera.Front);
		multipleLightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		multipleLightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		multipleLightingShader.setFloat("spotLight.constant", 1.0f);
		multipleLightingShader.setFloat("spotLight.linear", 0.09);
		multipleLightingShader.setFloat("spotLight.quadratic", 0.032);
		multipleLightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		multipleLightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		unsigned int diffuseMap = loadTexture("container2.png");
		unsigned int specularMap = loadTexture("container2_specular.png");
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
	}
}

// 渲染材质
void renderMaterial(int materialIndex, Shader lightingShader, Shader lampShader, int materialActive, unsigned int lightVAO) {
	if (materialActive == 1) {
		lightingShader.use();
		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular", 1);
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		if (materialIndex == 1) {
			lightingShader.setVec3("material.ambient", 0.329412f, 0.223529f, 0.027451f);
			lightingShader.setVec3("material.diffuse", 0.780392f, 0.568627f, 0.113725f);
			lightingShader.setVec3("material.specular", 0.992157f, 0.941176f, 0.807843f);
			lightingShader.setFloat("material.shininess", 0.21794872f);
		}

		if (materialIndex == 2) {
			lightingShader.setVec3("material.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("material.diffuse", 0.55f, 0.55f, 0.55f);
			lightingShader.setVec3("material.specular", 0.70f, 0.70f, 0.70f);
			lightingShader.setFloat("material.shininess", 0.25f);
		}

		if (materialIndex == 3) {
			lightingShader.setVec3("material.ambient", 0.1745f, 0.01175f, 0.01175f);
			lightingShader.setVec3("material.diffuse", 0.61424f, 0.04136f, 0.04136f);
			lightingShader.setVec3("material.specular", 0.727811f, 0.626959f, 0.626959f);
			lightingShader.setFloat("material.shininess", 0.6f);
		}
		if (materialIndex == 4) {
			glm::vec3 lightColor;
			lightColor.x = sin(glfwGetTime() * 2.0f);
			lightColor.y = sin(glfwGetTime() * 0.7f);
			lightColor.z = sin(glfwGetTime() * 1.3f);
			glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f); // decrease the influence
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f); // low influence
			lightingShader.setVec3("light.ambient", ambientColor);
			lightingShader.setVec3("light.diffuse", diffuseColor);
			lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			//Materials
			lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
			lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
			lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
			lightingShader.setFloat("material.shininess", 32.0f);
		}
	}
}

// 拖拽加载文件
void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	int i;
	for (i = 0; i < count; i++) {
		cout << paths[i] << endl;
		filePath = paths[i];
		cout << "new file loaded from Path" << filePath << endl;
		// 清除之前的向量
		VectorofVertice.clear();
		VectorofFace.clear();
		loadModelFromFile(filePath, VectorofVertice, VectorofFace);
		renderModel(VectorofVertice, VectorofFace, modelVBO, modelVAO, modelEBO);
	}
}

// 鼠标点击事件
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		std::cout << "Right button press" << std::endl;
		if (numOfLights >= 5) {
			std::cout << "reached maximum number of lights: 5; please decrease number of lights" << std::endl;
		}
		else {
			numOfLights = numOfLights + 1;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::cout << "Left button press" << std::endl;
		if (numOfLights <= 1) {
			std::cout << "reached minimum number of lights: 1: please increase number of lights" << std::endl;
		}
		else {
			numOfLights = numOfLights - 1;
		}
	}

	std::cout << "numOfLights" << numOfLights << std::endl;
}

// 动态渲染点光源
void dynamicRenderLights(int number, Shader lightingShader)
{
	if (number == 1) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
		};
		lightingShader.use();
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
	}

	if (number == 2) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
		};
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
	}

	if (number == 3) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
		};
		lightingShader.use();
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
	}

	if (number == 4) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
		};
		lightingShader.use();
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
	}

	if (number == 5) {
		glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f,  -1.0f, 1.0f)
		};
		lightingShader.use();
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// point light 5
		lightingShader.setVec3("pointLights[4].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[4].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[4].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[4].constant", 1.0f);
		lightingShader.setFloat("pointLights[4].linear", 0.09);
		lightingShader.setFloat("pointLights[4].quadratic", 0.032);
	}
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// 加载2D的材质，是一个PNG的图片
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// 从文件加载模型
void loadModelFromFile(string filePath, vector<float> &VectorofVertice, vector<unsigned int> &VectorofFace) {
	ifstream inFile;
	inFile.open(filePath, ios::in);
	if (inFile.fail())
	{
		cout << "Unable to open, please try again ! " << endl;
		system("pause");
		exit(1);
	}

	//ifstream inFile;
	//inFile.open(filePath, ios::in);

	char type;
	float x, y, z, nx, ny, nz;
	unsigned int f1, f2, f3;
	string Vertices, Faces;

	int NumofVertices, NumofFaces;
	float * vertices;


	unsigned int * faces;
	//vector<float> VectorofVertice;
	//vector<unsigned int> VectorofFace;

	inFile >> Vertices >> NumofVertices;
	inFile >> Faces >> NumofFaces;


	//dynamic array
	vertices = new float[NumofVertices * 6];  // 3 dimensions 顶点位置数据
	faces = new  unsigned int[NumofFaces * 3];  // 3 vertices each triangle 每个表面的数据

	// 顶点数据
	for (int i = 0; i < NumofVertices; i++)
	{
		inFile >> type >> x >> y >> z >> nx >> ny >> nz;

		vertices[3 * i] = x;
		vertices[3 * i + 1] = y;
		vertices[3 * i + 2] = z;
		vertices[3 * i + 3] = nx;
		vertices[3 * i + 4] = ny;
		vertices[3 * i + 5] = nz;

		VectorofVertice.push_back(x);
		VectorofVertice.push_back(y);
		VectorofVertice.push_back(z);
		VectorofVertice.push_back(nx);
		VectorofVertice.push_back(ny);
		VectorofVertice.push_back(nz);
	}

	// 表面数据
	for (int j = 0; j < NumofFaces; j++)
	{
		inFile >> type >> f1 >> f2 >> f3;
		f1 -= 1;
		f2 -= 1;
		f3 -= 1;
		VectorofFace.push_back(f1);
		VectorofFace.push_back(f2);
		VectorofFace.push_back(f3);
	}
	cout << VectorofVertice.size() << endl;

	inFile.close();
	return;
}

void renderModel(vector<float> VectorofVertice, vector<unsigned int> VectorofFace, unsigned &modelVBO, unsigned &modelVAO, unsigned &modelEBO) {
	glGenVertexArrays(1, &modelVAO);
	glGenBuffers(1, &modelVBO);
	glGenBuffers(1, &modelEBO);
	// 首先绑定顶点数据 在绑定顶点buffer 再配置顶点
	glBindVertexArray(modelVAO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
	glBufferData(GL_ARRAY_BUFFER, int(VectorofVertice.size()) * sizeof(float), &VectorofVertice[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, int(VectorofFace.size()) * sizeof(float), &VectorofFace[0], GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

