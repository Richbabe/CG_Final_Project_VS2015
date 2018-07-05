#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>
#include <Camera.h>
#include "Skybox.h"
#include "model.h"
#include "Sphere.h"
#include "AABB.h"
#include "ptext.h"

#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
Model ship(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool shadows = true;
bool shadowsKeyPressed = false;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool ShowAABB = 0;
bool gameOver = false;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	glEnable(GL_CULL_FACE);

	// build and compile shaders
	// -------------------------
	Shader shader("cubemap.vs", "cubemap.fs");
	Shader normalShader("shader.vs", "shader.fs");
	Shader skyboxShader("skybox.vs", "skybox.fs");
	Shader modelShader("model.vs", "model.fs");
	Shader AABBShader("AABBshader.vs", "AABBshader.fs");
	Shader simpleDepthShader("depth.vs", "depth.fs", "depth.gs");
	Shader sunShader("sunShader.vs", "sunShader.fs");

	test_vao(SCR_WIDTH, SCR_HEIGHT);//text

	Model shipModel(std::string("resources/objects/F15C/F-15C_Eagle.dae"));// loading the model

	AABB shipAABB(shipModel);//∑…¥¨AABB≈ˆ◊≤ƒ£–Õ

	/*cout << "ship:" << endl;
	cout << "xMin:" << shipAABB.xMin << " xMax:" << shipAABB.xMax << endl;
	cout << "yMin:" << shipAABB.yMin << " yMax:" << shipAABB.yMax << endl;
	cout << "zMin:" << shipAABB.zMin << " zMax:" << shipAABB.zMax << endl;
	cout << "*****************" << endl;*/

	//–°–––«¥¯
	Shader asteroidShader("asteroids.vs", "asteroids.fs");
	Model rock(std::string("resources/objects/rock/rock.obj"));
	unsigned int amount = 10000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 16.0f;
	float offset = 3.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 500.0f + 0.005;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int VAO = rock.meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	// Sphere
	// ------
	int divide = 100;
	Sphere spheres[11];
	float raidus[11] = { 0.80, 0.10, 0.20, 0.25, 0.05, 0.15, 0.30, 0.35, 0.35, 0.40, 0.10};
	vector<string> textureMap
	{
		string("resources/textures/planet/Ã´—Ù.jpg"),
		string("resources/textures/planet/ÀÆ–«.jpg"),
		string("resources/textures/planet/Ω–«.jpg"),
		string("resources/textures/planet/µÿ«Ú.jpg"),
		string("resources/textures/planet/‘¬«Ú.jpg"),
		string("resources/textures/planet/ª–«.jpg"),
		string("resources/textures/planet/ƒæ–«.jpg"),
		string("resources/textures/planet/Õ¡–«.jpg"),
		string("resources/textures/planet/ÃÏÕı–«.jpg"),
		string("resources/textures/planet/∫£Õı–«.jpg"),
		string("resources/textures/planet/⁄§Õı–«.jpg"),
	};

	for (int i = 0; i < 11; i++)
		spheres[i] = Sphere(divide, raidus[i], textureMap[i]);

	AABB spheresAABB[11];//–««Ú≈ˆ◊≤∫–
	for (int i = 0; i < 11; i++) {
		spheresAABB[i] = AABB(spheres[i]);
	}
	
	// load textures
	// -------------
	vector<std::string> faces
	{
		std::string("resources/textures/space_skybox/right.tga"),
		std::string("resources/textures/space_skybox/left.tga"),
		std::string("resources/textures/space_skybox/top.tga"),
		std::string("resources/textures/space_skybox/bottom.tga"),
		std::string("resources/textures/space_skybox/back.tga"),
		std::string("resources/textures/space_skybox/front.tga"),
	};
	Skybox skybox(faces);
	
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth cubemap texture
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("skybox", 0);

	normalShader.use();
	normalShader.setInt("diffuseTexture", 0);
	normalShader.setInt("depthMap", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	int count = 0;

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		count++;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		float near_plane = 1.0f;
		float far_plane = 1000.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		


		// view/projection transformations
		glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 shipView = camera.GetShipViewMatrix(ship.Position);
		//renderScene(normalShader, AABBShader,modelShader, spheres, spheresAABB, shipAABB, shipModel, count);


		// draw sun
		glm::mat4 sun_trans(1);//±‰ªªæÿ’Û

		// draw mercury
		glm::mat4 mercury_trans(1);//±‰ªªæÿ’Û
		mercury_trans = glm::rotate(mercury_trans, (float)glfwGetTime() * 10 * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mercury_trans = glm::translate(mercury_trans, glm::vec3(1.5, 0.0, 0.0));
		mercury_trans = glm::rotate(mercury_trans, (float)glfwGetTime() * 10 * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));

		// draw venus
		glm::mat4 venus_trans(1);//±‰ªªæÿ’Û
		venus_trans = glm::rotate(venus_trans, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		venus_trans = glm::translate(venus_trans, glm::vec3(2.5, 0.0, 0.0));
		venus_trans = glm::rotate(venus_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		
		// draw earth
		static glm::vec4 earth_position(0.0, 0.0, 0.0, 1.0);
		glm::mat4 earth_trans(1);//±‰ªªæÿ’Û
		earth_trans = glm::rotate(earth_trans, (float)glfwGetTime() * glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		earth_trans = glm::translate(earth_trans, glm::vec3(3.5, 0.0, 0.0));
		earth_position = earth_trans * earth_position;
		earth_trans = glm::rotate(earth_trans, (float)glfwGetTime() * glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// draw moon
		int earth_moon_radium = 0.2;
		//int dx = earth_moon_radium * cos((float)glfwGetTime());
		//int dz = earth_moon_radium * sin((float)glfwGetTime());
		glm::mat4 moon_trans = earth_trans;//±‰ªªæÿ’Û
		//moon_trans = glm::rotate(moon_trans, (float)glfwGetTime() * 10 * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		moon_trans = glm::translate(moon_trans, glm::vec3(0.8, 0.0, 0.0));
		//moon_trans =
		//moon_trans = glm::translate(moon_trans, glm::vec3(dx + earth_position.x, 0, dz + earth_position.z));
		//moon_trans = glm::rotate(moon_trans, (float)glfwGetTime() * 10 * glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		

		// draw mars
		glm::mat4 mars_trans(1);//±‰ªªæÿ’Û
		mars_trans = glm::rotate(mars_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mars_trans = glm::translate(mars_trans, glm::vec3(5.0, 0.0, 0.0));
		mars_trans = glm::rotate(mars_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		

		// draw jupiter
		glm::mat4 jupiter_trans(1);//±‰ªªæÿ’Û
		jupiter_trans = glm::rotate(jupiter_trans, (float)glfwGetTime() * glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		jupiter_trans = glm::translate(jupiter_trans, glm::vec3(6.2, 0.0, 0.0));
		jupiter_trans = glm::rotate(jupiter_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		

		// draw saturn
		glm::mat4 saturn_trans(1);//±‰ªªæÿ’Û
		saturn_trans = glm::rotate(saturn_trans, (float)glfwGetTime() * glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		saturn_trans = glm::translate(saturn_trans, glm::vec3(7.6, 0.0, 0.0));
		saturn_trans = glm::rotate(saturn_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));

		// draw uranus
		glm::mat4 uranus_trans(1);//±‰ªªæÿ’Û
		uranus_trans = glm::rotate(uranus_trans, (float)glfwGetTime() * glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		uranus_trans = glm::translate(uranus_trans, glm::vec3(8.9, 0.0, 0.0));
		uranus_trans = glm::rotate(uranus_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));

		// draw neptune
		glm::mat4 neptune_trans(1);//±‰ªªæÿ’Û
		neptune_trans = glm::rotate(neptune_trans, (float)glfwGetTime() * glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		neptune_trans = glm::translate(neptune_trans, glm::vec3(10.5, 0.0, 0.0));
		neptune_trans = glm::rotate(neptune_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		

		// draw pluto
		glm::mat4 pluto_trans(1);//±‰ªªæÿ’Û
		pluto_trans = glm::rotate(pluto_trans, (float)glfwGetTime() * glm::radians(130.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		pluto_trans = glm::translate(pluto_trans, glm::vec3(12, 0.0, 0.0));
		pluto_trans = glm::rotate(pluto_trans, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 1.0f));

		//ªÊ÷∆…Ó∂»Ã˘Õº
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		simpleDepthShader.use();
		for (unsigned int i = 0; i < 6; ++i)
			simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		simpleDepthShader.setFloat("far_plane", far_plane);
		simpleDepthShader.setVec3("lightPos", lightPos);
		spheres[0].Draw(simpleDepthShader, sun_trans, view, projection);
		spheres[1].Draw(simpleDepthShader, mercury_trans, view, projection);
		spheres[2].Draw(simpleDepthShader, venus_trans, view, projection);
		spheres[3].Draw(simpleDepthShader, earth_trans, view, projection);
		spheres[4].Draw(simpleDepthShader, moon_trans, view, projection);
		spheres[5].Draw(simpleDepthShader, mars_trans, view, projection);
		spheres[6].Draw(simpleDepthShader, jupiter_trans, view, projection);
		spheres[7].Draw(simpleDepthShader, saturn_trans, view, projection);
		spheres[8].Draw(simpleDepthShader, uranus_trans, view, projection);
		spheres[9].Draw(simpleDepthShader, neptune_trans, view, projection);
		spheres[10].Draw(simpleDepthShader, pluto_trans, view, projection);

		simpleDepthShader.use();
		simpleDepthShader.setMat4("projection", projection);
		simpleDepthShader.setMat4("view", shipView);
		glm::mat4 transform(1);
		transform = glm::scale(transform, glm::vec3(0.0001f, 0.0001f, 0.0001f));    // it's a bit too big for our scene, so scale it down
																			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
																			//cout << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z  << " " << endl;
																			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f)); // translate it down so it's at the center of the scene
		transform = glm::translate(transform, ship.Position / 0.0001f); // translate it down so it's at the center of the scene
		transform = glm::rotate(transform, -camera.Yaw - 90, glm::vec3(0.0, 1.0, 0.0));
		transform = glm::rotate(transform, camera.Pitch, glm::vec3(1.0, 0.0, 0.0));
		camera.SetPosition(glm::vec3(ship.Position.x, ship.Position.y, ship.Position.z));
		//cout << camera.Yaw << endl;
		simpleDepthShader.setMat4("transform", transform);
		shipModel.Draw(simpleDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//‰÷»æ≥°æ∞“‘º∞“ı”∞
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		normalShader.use();
		normalShader.setInt("reverse_normals", false);
		// set lighting uniforms
		normalShader.setVec3("lightPos", lightPos);
		normalShader.setVec3("viewPos", camera.Position);
		normalShader.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
		normalShader.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		spheres[0].Draw(sunShader, sun_trans, shipView, projection);
		spheres[1].Draw(normalShader, mercury_trans, shipView, projection);
		spheres[2].Draw(normalShader, venus_trans, shipView, projection);
		spheres[3].Draw(normalShader, earth_trans, shipView, projection);
		spheres[4].Draw(normalShader, moon_trans, shipView, projection);
		spheres[5].Draw(normalShader, mars_trans, shipView, projection);
		spheres[6].Draw(normalShader, jupiter_trans, shipView, projection);
		spheres[7].Draw(normalShader, saturn_trans, shipView, projection);
		spheres[8].Draw(normalShader, uranus_trans, shipView, projection);
		spheres[9].Draw(normalShader, neptune_trans, shipView, projection);
		spheres[10].Draw(normalShader, pluto_trans, shipView, projection);
		/*
		spheresAABB[0].drawAABB(AABBShader, sun_trans, shipView, projection, ShowAABB);//ªÊ÷∆Ã´—ÙAABB≈ˆ◊≤∫–
		spheresAABB[1].drawAABB(AABBShader, mercury_trans, shipView, projection, ShowAABB);
		spheresAABB[2].drawAABB(AABBShader, venus_trans, shipView, projection, ShowAABB);
		spheresAABB[3].drawAABB(AABBShader, earth_trans, shipView, projection, ShowAABB);
		spheresAABB[4].drawAABB(AABBShader, moon_trans, shipView, projection, ShowAABB);
		spheresAABB[5].drawAABB(AABBShader, mars_trans, shipView, projection, ShowAABB);
		spheresAABB[6].drawAABB(AABBShader, jupiter_trans, shipView, projection, ShowAABB);
		spheresAABB[7].drawAABB(AABBShader, saturn_trans, shipView, projection, ShowAABB);
		spheresAABB[8].drawAABB(AABBShader, uranus_trans, shipView, projection, ShowAABB);
		spheresAABB[9].drawAABB(AABBShader, neptune_trans, shipView, projection, ShowAABB);
		spheresAABB[10].drawAABB(AABBShader, pluto_trans, shipView, projection, ShowAABB);
		*/
		modelShader.use();
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", shipView);
		glm::mat4 model(1);
		model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));    // it's a bit too big for our scene, so scale it down
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
		//cout << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z  << " " << endl;
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f)); // translate it down so it's at the center of the scene
		model = glm::translate(model, ship.Position / 0.0001f); // translate it down so it's at the center of the scene
		model = glm::rotate(model, -camera.Yaw - 90, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, camera.Pitch, glm::vec3(1.0, 0.0, 0.0));
		camera.SetPosition(glm::vec3(ship.Position.x, ship.Position.y, ship.Position.z));
		//cout << camera.Yaw << endl;
		modelShader.setMat4("model", model);
		shipModel.Draw(modelShader);

		//ªÊ÷∆∑…¥¨AABBƒ£–Õ
		AABBShader.use();
		shipAABB.drawAABB(AABBShader, model, shipView, projection, ShowAABB);

		//ª≠–°–––«¥¯
		asteroidShader.use();
		asteroidShader.setMat4("projection", projection);
		asteroidShader.setMat4("view", shipView);

		asteroidShader.use();
		asteroidShader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}
		/*
		cout << "ship:" << endl;
		cout << "xMin:" << shipAABB.xMin << " xMax:" << shipAABB.xMax << endl;
		cout << "yMin:" << shipAABB.yMin << " yMax:" << shipAABB.yMax << endl;
		cout << "zMin:" << shipAABB.zMin << " zMax:" << shipAABB.zMax << endl;
		cout << "-----------------" << endl;
		*/

		bool isIntersection = false;
		//ºÏ≤‚∑…¥¨ «∑Ò”Î–««Ú≈ˆ◊≤
		for (int i = 0; i < 11; i++) {
			if (shipAABB.IsIntersection(spheres[i])) {
				isIntersection = true;
				RenderText("GameOver!", 525.0f, 525.0f, 3.0f, glm::vec3(1.0f, 0.0f, 0.0f), SCR_WIDTH, SCR_HEIGHT);

				//cout << "crash ball " << i << " game over!!!" << endl;
			}
		}

		gameOver = isIntersection;

		skybox.Draw(skyboxShader, view, projection);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (gameOver == false) {
		//cout << "last: " << ship.Position.x << " " << ship.Position.y << " " << ship.Position.z << " ";
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			ship.ProcessKeyboard(FORWARD, deltaTime, camera.Front);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			ship.ProcessKeyboard(BACKWARD, deltaTime, camera.Front);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			ship.ProcessKeyboard(LEFT, deltaTime, camera.Front);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			ship.ProcessKeyboard(RIGHT, deltaTime, camera.Front);
		//cout << "next: " << ship.Position.x << " " << ship.Position.y << " " << ship.Position.z << endl;
	}

	//∞¥œ¬Mœ‘ æ/“˛≤ÿ≈ˆ◊≤∫–
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		ShowAABB = 1 - ShowAABB;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		ship.Position = glm::vec3(0.0f, 0.0f, 8.0f);
		gameOver = false;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
	{
		shadows = !shadows;
		shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadowsKeyPressed = false;
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

	if (gameOver == false)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}