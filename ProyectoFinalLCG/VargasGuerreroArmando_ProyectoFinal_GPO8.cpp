#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(28.836231f, 4.287041f, 6.56f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;


// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
//glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;
glm::vec3 PosIni(20.836231f, 4.287041f, 6.56f);

//Animación puerta
float rotPuerta = 0.0;
bool puertaAbierta = false;

//Animación del coche
float movKitX = 0.0;
float movKitZ = 0.0;
float movX = 0.0;
float rotKit = 0.0;

bool circuito = false;
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;
bool recorrido4 = false;
bool recorrido5 = false;
bool recorrido6 = false;
bool recorrido7 = false;
bool recorrido8 = false;
bool recorrido9 = false;
bool recorrido10 = false;
bool recorrido11 = false;
bool recorrido12 = false;
bool recorrido13 = false;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(20.0f,  10.0f,  4.0f),
	glm::vec3(0.0f,  10.0f,  4.0f),
	glm::vec3(-20.0f,  10.0f,  4.0f),
	glm::vec3(-20.0f,  30.0f,  4.0f)
};

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");

	//Model cjHouse((char*)"Models/cjHouse/cjHouse.obj");


	Model cjHouse2((char*)"Models/cjHouse4/cjHouse_rec.obj");
	//Model cjHouse2((char*)"Models/cjHouseFinal/cjHouseFinal.obj");
	Model Carroseria((char*)"Models/Chrysler/chrysler.obj");
	Model LlantaIzqSup((char*)"Models/Chrysler/llantaSupIzq.obj");
	Model LlantaDerSup((char*)"Models/Chrysler/llantaSupDer.obj");
	Model LlantaIzqInf((char*)"Models/Chrysler/llantaInfIzq.obj");
	Model LlantaDerInf((char*)"Models/Chrysler/llantaInfDer.obj");
	Model PuertaPrinc((char*)"Models/Puerta/puertaPrinc.obj");


	// Build and compile our shader program

	//Modelo de animación
	/*ModelAnim animacionPersonaje("Animaciones/FistFight/FistFight.dae");*/

	//animacionPersonaje.initShaders(animShader.Program);
	//Inicialización de KeyFrames
	
	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotInc = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	/*GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);*/

	// Load textures
	/*vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);*/

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.501f, 0.823f, 0.878f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		//Carga de modelo 

		/*Casa*/
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(-2.5f, -3.2f, -0.6f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cjHouse2.Draw(lightingShader);

		//Puerta principal
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		model = glm::translate(model, glm::vec3(-3.6f, 0.0f, 6.8));
		model = glm::rotate(model, glm::radians(rotPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(3.6f, 0.0f, -6.8f));
		PuertaPrinc.Draw(lightingShader);

		//Carroceria
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//model = glm::translate(model, PosIni + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Carroseria.Draw(lightingShader);

		//Llanta Delantera Der
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//model = glm::translate(model, PosIni + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(1.7f, 0.5f, 2.6f));;
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerSup.Draw(lightingShader);

		//Llanta Trasera Der
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//model = glm::translate(model, PosIni + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(1.7f, 0.5f, -2.9f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerInf.Draw(lightingShader);


		//Llanta Delantera Izq
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//model = glm::translate(model, PosIni + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(-1.7f, 0.8f, 2.6f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzqSup.Draw(lightingShader);

		//Llanta Trasera Izq
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//model = glm::translate(model, PosIni + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(-1.7f, 0.8f, -2.9f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzqInf.Draw(lightingShader);
		glBindVertexArray(0);

		/*_______________________________Personaje Animado___________________________*/ 
		//animShader.Use();
		//modelLoc = glGetUniformLocation(animShader.Program, "model");
		//viewLoc = glGetUniformLocation(animShader.Program, "view");
		//projLoc = glGetUniformLocation(animShader.Program, "projection");

		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		//glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
		//glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 0.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.5f, 0.5f, 0.5f);
		//glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"),0.0f, -1.0f, -1.0f);
		//view = camera.GetViewMatrix();

		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(PosIni.x+5.0f,PosIni.y-1.0f,PosIni.z));
		//model = glm::scale(model, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//animacionPersonaje.Draw(animShader);
		//glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		//glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		//SkyBoxshader.Use();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		//glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//// skybox cube
		//glBindVertexArray(skyboxVAO);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		//glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	//glDeleteVertexArrays(1, &skyboxVAO);
	//glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{

		//Movimiento del personaje

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex>FrameIndex - 2)	//end of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posX += KeyFrame[playIndex].incX;
				posY += KeyFrame[playIndex].incY;
				posZ += KeyFrame[playIndex].incZ;

				rotRodIzq += KeyFrame[playIndex].rotInc;

				i_curr_steps++;
			}

		}
	}

float slope(char varIndep, float posIniZ, float posFinZ, float posIniX, float posFinX) {
	float deltaX = posFinX - posIniX;
	float deltaZ = posFinZ - posIniZ;
	if (varIndep == 'z') {

		return float(deltaX / deltaZ);
	}
	else if (varIndep == 'x') {
		return float(deltaZ / deltaX);
	}

	return -1.0;
}


void animacionCoche()
{
	//float movX = 0.0f;
	//Movimiento circular del coche
	if (circuito)
	{
		if (recorrido1)
		{
			rotKit = 30;
			//slope(char varIndep, float posIniZ, float posFinZ, float posIniX, float posFinX) 
			movKitZ = slope('x', 6.5, 25.5, 20.8, 9.9) * movX + 6.5;
			movX -= 0.1f;
			movKitX -= movX;
			printf("R1: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ > 25.5)
			{
				printf("\n");
				/*recorrido1 = false;
				recorrido2 = true;*/
				circuito = false;
			}
		}
		if (recorrido2)
		{
			rotKit = 30;
			movX = 0.0f;

			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', 25.5, 36.5, 9.9, -9.2) * movX + 25.5;
			movX -= 0.1f;
			movKitX -= movX;
			printf("R2: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);

			if (movKitZ > 36.5)
			{
				recorrido2 = false;
				recorrido3 = true;

			}
		}

		if (recorrido3)
		{
			rotKit = 30;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			//movKitX = 0.0f;
			//movKitZ = slope('x', 36.5, 36.5, -9.2, -31.1) * movKitX + 36.5;
			movKitX -= 0.1f;
			printf("R3: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitX < -31.1)
			{
				recorrido3 = false;
				recorrido4 = true;
			}
		}

		if (recorrido4)
		{
			rotKit = 30;
			movX = 0.0f;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', 36.5, 25.5, -31.1, -50.2) * movX + 36.5;
			movKitX -= 0.1f;
			movKitX -= movX;
			printf("R4: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ < 25.5)
			{
				recorrido4 = false;
				recorrido5 = true;
			}
		}

		if (recorrido5)
		{
			rotKit = 30;
			movX = 0.0f;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', 25.5, 6.5, -50.2, -61.2) * movX + 25.5;
			movX -= 0.1f;
			movKitX -= movX;
			printf("R5: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ < 6.5)
			{
				recorrido5 = false;
				recorrido6 = true;
			}
		}

		if (recorrido6)
		{
			movX = 0.0f;
			rotKit = 30;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			//movKitZ = slope('x', 6.5, -15.5, -61.2, -61.2) * movKitX + 6.5;
			movKitZ -= 0.1f;
			//movKitX -= 0.1f;
			printf("R6: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ < -15.5)
			{
				recorrido6 = false;
				recorrido7 = true;
			}
		}

		if (recorrido7)
		{
			movX = 0.0f;
			rotKit = 30;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', -15.5, -34.5, -61.2, -50.2) * movX - 15.5;
			movX += 0.1f;
			movKitX += movX;
			printf("R7: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ < -34.5)
			{
				recorrido7 = false;
				recorrido8 = true;
			}
		}

		if (recorrido8)
		{
			rotKit = 30;
			movX = 0.0;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', -34.5, -45.5, -50.2, -31.1) * movX - 34.5;
			movKitX += 0.1f;
			movKitX += movX;
			printf("R8: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ < -45.5)
			{
				recorrido8 = false;
				recorrido9 = true;
			}
		}

		if (recorrido9)
		{
			rotKit = 30;
			//movX = 0.0f;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			//movKitZ = slope('x', -45.5, -45.5, -31.1, -9.2) * movX - 45.5;
			movKitX += 0.1f;
			//movKitX += movX;
			printf("R9: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitX > -9.2)
			{
				recorrido9 = false;
				recorrido10 = true;
			}
		}

		if (recorrido10)
		{
			rotKit = 30;
			movX = 0.0f;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', -45.5, -34.5, -9.2, 9.9) * movX - 45.5;
			movX += 0.1f;
			movKitX += movX;
			printf("R10: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ > -34.5)
			{
				recorrido10 = false;
				recorrido11 = true;
			}
		}

		if (recorrido11)
		{
			rotKit = 30;
			movX = 0.0f;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			movKitZ = slope('x', -34.5, -15.5, 9.9, 20.8) * movX - 34.5;
			movX += 0.1f;
			movKitX += movX;
			printf("R11: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ > -15.5)
			{
				recorrido11 = false;
				recorrido12 = true;
			}
		}

		if (recorrido12)
		{
			rotKit = 30;
			//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
			//movKitZ = slope('x', -15.5, 6.5, 5.7, 18.2) * movKitX;
			movKitZ += 0.1f;
			printf("R12: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
			if (movKitZ > 6.5)
			{
				recorrido12 = false;
				recorrido1 = true;
			}
		}

		//if (recorrido13)
		//{
		//	rotKit = 0;
		//	//slope(char varIndep, float posIniX, float posFinX, float posIniZ, float posFinZ) 
		//	movKitZ = slope('x', -15.3, 6.54, 18.2, 20.8) * movKitX;
		//	movKitX += 0.1f;
		//	printf("R13: movKitX: %f, movKitZ: %f \n", movKitX, movKitZ);
		//	if (movKitX > 20.8)
		//	{
		//		recorrido13 = false;
		//		recorrido1 = true;
		//	}
		//}


	}
}



// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{

		rot += 1;

	}

	if (keys[GLFW_KEY_2])
	{
		if (rotRodIzq<80.0f)
			rotRodIzq += 1.0f;
			
	}

	if (keys[GLFW_KEY_3])
	{
		if (rotRodIzq>-45)
			rotRodIzq -= 1.0f;
		
	}

	if (keys[GLFW_KEY_E])
	{
		if (rotPuerta >= -85.0) {
			rotPuerta -= 10.0;
			printf("%f", rotPuerta);
			//puertaAbierta = true;
		}

		//if (rotPuerta <= 90.0) {
		//	rotPuerta += 18.0;
		//	//puertaAbierta = true;
		//}
		/*else {
			rotPuerta += 18.0;
			puertaAbierta = false;
		}*/
	}

	if (keys[GLFW_KEY_F])
	{
		if (rotPuerta <= -5.0) {
			rotPuerta += 10.0;
			printf("%f", rotPuerta);
			//puertaAbierta = true;
		}

		//if (rotPuerta <= 90.0) {
		//	rotPuerta += 18.0;
		//	//puertaAbierta = true;
		//}
		/*else {
			rotPuerta += 18.0;
			puertaAbierta = false;
		}*/
	}
	

	//Mov Personaje
	if (keys[GLFW_KEY_H])
	{
		posZ += 1;
	}

	if (keys[GLFW_KEY_Y])
	{
		posZ -= 1;
	}

	if (keys[GLFW_KEY_G])
	{
		posX -= 1;
	}

	if (keys[GLFW_KEY_J])
	{
		posX += 1;
	}

	if (keys[GLFW_KEY_O])
		circuito = true;

	if (keys[GLFW_KEY_P])
		circuito = false;


	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	




}