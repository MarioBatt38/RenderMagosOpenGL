
#include "core.h"
#include "TextureLoader.h"
#include "shader_setup.h"
#include "ArcballCamera.h"
#include "GUClock.h"
#include "AIMesh.h"



using namespace std;
using namespace glm;


struct DirectionalLight {

	vec3 direction;
	vec3 colour;
	
	DirectionalLight() {

		direction = vec3(0.0f,  1.0f, 0.0f); // default to point upwards
		colour = vec3(1.0f, 1.0f, 1.0f);
	}

	DirectionalLight(vec3 direction, vec3 colour = vec3(1.0f, 1.0f, 1.0f)) {

		this->direction = direction;
		this->colour = colour;
	}
};

struct PointLight {

	vec3 pos;
	vec3 colour;
	vec3 attenuation; // x=constant, y=linear, z=quadratic

	PointLight() {

		pos = vec3(0.0f, 0.0f, 0.0f);
		colour = vec3(1.0f, 1.0f, 1.0f);
		attenuation = vec3(1.0f, 1.0f, 1.0f);
	}

	PointLight(vec3 pos, vec3 colour = vec3(1.0f, 1.0f, 1.0f), vec3 attenuation = vec3(1.0f, 1.0f, 1.0f)) {

		this->pos = pos;
		this->colour = colour;
		this->attenuation = attenuation;
	}


};





#pragma region Global variables

// Window size
unsigned int		windowWidth = 1920;
unsigned int		windowHeight = 1080;

// Main clock for tracking time (for animation / interaction)
GUClock*			gameClock = nullptr;

// Main camera
ArcballCamera*		mainCamera = nullptr;

// Mouse tracking
bool				mouseDown = false;
double				prevMouseX, prevMouseY;

// Keyboard tracking
bool				forwardPressed;
bool				backPressed;
bool				rotateLeftPressed;
bool				rotateRightPressed;
bool				starForgerSelected=true;
bool				starWalkerSelected = false;

//
// Scene objects
//

AIMesh*				groundMesh = nullptr;
AIMesh*				creatureMesh = nullptr;
AIMesh*				columnMesh = nullptr;


//Magos Terrain
AIMesh*				magosgroundMesh = nullptr;
AIMesh*				magosBoardMesh = nullptr;
AIMesh*				magosMapBoardMesh = nullptr;
AIMesh*				magosShrineBoardMesh = nullptr;
AIMesh*				magosSingleRockMesh = nullptr;
AIMesh*				magosClusterRockMesh = nullptr;
AIMesh*				magosBlackSandTileMesh = nullptr;
AIMesh*				magosMapMesh = nullptr;
AIMesh*				magosMapMesh2 = nullptr;
AIMesh*				magosBlackSandMesh = nullptr;
AIMesh*				magosCrystalMesh = nullptr;

//Magos Buildings
AIMesh*				astraOutpostMesh = nullptr;
AIMesh*				astraSettlementMesh = nullptr;
AIMesh*				astraBrigadeMesh = nullptr;

//Magos Background
AIMesh*		magosSpaceBGMesh = nullptr;



//Magos Characters
AIMesh*				starForgerMesh = nullptr;
AIMesh*             starWalkerMesh = nullptr;


// Shaders

// Basic colour shader
GLuint				basicShader;
GLint				basicShader_mvpMatrix;

// Texture-directional light shader
GLuint				texDirLightShader;
GLint				texDirLightShader_modelMatrix;
GLint				texDirLightShader_viewMatrix;
GLint				texDirLightShader_projMatrix;
GLint				texDirLightShader_texture;
GLint				texDirLightShader_lightDirection;
GLint				texDirLightShader_lightColour;

// Texture-point light shader
GLuint				texPointLightShader;
GLint				texPointLightShader_modelMatrix;
GLint				texPointLightShader_viewMatrix;
GLint				texPointLightShader_projMatrix;
GLint				texPointLightShader_texture;
GLint				texPointLightShader_lightPosition;
GLint				texPointLightShader_lightColour;
GLint				texPointLightShader_lightAttenuation;

//  *** normal mapping *** Normal mapped texture with Directional light
// This is the same as the texture direct light shader above, but with the addtional uniform variable
// to set the normal map sampler2D variable in the fragment shader.
GLuint				nMapDirLightShader;
GLint				nMapDirLightShader_modelMatrix;
GLint				nMapDirLightShader_viewMatrix;
GLint				nMapDirLightShader_projMatrix;
GLint				nMapDirLightShader_diffuseTexture;
GLint				nMapDirLightShader_normalMapTexture;
GLint				nMapDirLightShader_lightDirection;
GLint				nMapDirLightShader_lightColour;




// cylinder model //delete
vec3 cylinderPos = vec3(-2.0f, 2.0f, 0.0f);

// StarForger model
vec3 starForgerPos = vec3(1.0f, 0.0f, -10.0f);
float starForgerRotation = 0.0f;

// StarForger model
vec3 starWalkerPos = vec3(7.0f, 0.0f, 0.0f);
float starWalkerRotation = 0.0f;

// Directional light Setup
float directLightTheta = 6.0f;
DirectionalLight directLight = DirectionalLight(vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f));

// Point Light setup
PointLight lights[1] = {
	PointLight(vec3(-1.0f, 7.0f, -1.0f), vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 0.1f, 0.001f))

};



bool rotateDirectionalLight = true;





#pragma endregion


// Functions
void renderScene();

void PopulusMagosLevel1();
void PopulusMagosLevel2();
void updateScene();
void resizeWindow(GLFWwindow* window, int width, int height);
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos);
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods);
void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset);
void mouseEnterHandler(GLFWwindow* window, int entered);



int main() {

	//
	// 1. Initialisation
	//
	
	gameClock = new GUClock();

#pragma region OpenGL and window setup

	// Initialise glfw and setup window
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "CIS5013", NULL, NULL);

	// Check window was created successfully
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	

	// Set callback functions to handle different events
	glfwSetFramebufferSizeCallback(window, resizeWindow); // resize window callback
	glfwSetKeyCallback(window, keyboardHandler); // Keyboard input callback
	glfwSetCursorPosCallback(window, mouseMoveHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetScrollCallback(window, mouseScrollHandler);
	glfwSetCursorEnterCallback(window, mouseEnterHandler);

	// Initialise glew
	glewInit();

	
	// Setup window's initial size
	resizeWindow(window, windowWidth, windowHeight);

#pragma endregion


	// Initialise scene - geometry and shaders etc
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // setup background colour to be black
	glClearDepth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	//
	// Setup Textures, VBOs and other scene objects
	//
	mainCamera = new ArcballCamera(-45.0f, 230.0f, 30.0f, 75.0f, (float)windowWidth/(float)windowHeight, 0.1f, 5000.0f);
	
	

	//
	//Characters
	//
	starForgerMesh = new AIMesh(string("Assets\\StarForger\\StarForger.obj"));
	if (starForgerMesh) {
		starForgerMesh->addTexture(string("Assets\\StarForger\\StarForger_Texture.bmp"), FIF_BMP);

	}

	starWalkerMesh = new AIMesh(string("Assets\\StarWalker\\StarWalker_Mesh.obj"));
	if (starWalkerMesh) {
		starWalkerMesh->addTexture(string("Assets\\StarWalker\\\StarWalker_Texture.bmp"), FIF_BMP);

	}

	//
	//Buildings
	//
	
	columnMesh = new AIMesh(string("Assets\\column\\Column.obj"));
	if (columnMesh) {
		columnMesh->addTexture(string("Assets\\column\\column_d.bmp"), FIF_BMP);
		columnMesh->addNormalMap(string("Assets\\column\\column_n.bmp"), FIF_BMP);
	}

	astraOutpostMesh = new AIMesh(string("Assets\\MagosAstraBuildings\\Astra_Outpost_Mesh.obj"));
	if (astraOutpostMesh) {
		astraOutpostMesh->addTexture(string("Assets\\MagosAstraBuildings\\Astra_Buildings_Textures.bmp"), FIF_BMP);
	}



	astraBrigadeMesh = new AIMesh(string("Assets\\MagosAstraBuildings\\Astra_Brigade_Mesh.obj"));
	if (astraBrigadeMesh) {
		astraBrigadeMesh->addTexture(string("Assets\\MagosAstraBuildings\\Astra_Buildings_Textures.bmp"), FIF_BMP);
	}


	//
	//Terrain
	//

	magosgroundMesh = new AIMesh(string("Assets\\MagosGroundPlane\\magosGround_Mesh.obj"));
	if (magosgroundMesh) {
		magosgroundMesh->addTexture(string("Assets\\MagosGroundPlane\\MagosTerrain_Texture_DiffuseMap.bmp"), FIF_BMP);
		magosgroundMesh->addNormalMap(string("Assets\\MagosGroundPlane\\MagosTerrain_Texture_NormalMap.bmp"), FIF_BMP);
	}

	magosBoardMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\MagosTerrainBoard_Mesh.obj"));
	if (magosBoardMesh) {
		magosBoardMesh->addTexture(string("Assets\\MagosTerrainBoard\\MagosTerrainBoard_Texture.bmp"), FIF_BMP);
		
	}

	magosMapBoardMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\MagosMapBoard_Mesh.obj"));
	if (magosBoardMesh) {
		magosBoardMesh->addTexture(string("Assets\\MagosTerrainBoard\\MagosTerrainBoard_Texture.bmp"), FIF_BMP);

	}


	magosShrineBoardMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\MagosShrineBoard_Mesh.obj"));
	if (magosShrineBoardMesh) {
		magosShrineBoardMesh->addTexture(string("Assets\\MagosTerrainBoard\\MagosTerrainBoard_Texture.bmp"), FIF_BMP);

	}

	magosSingleRockMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\MagosSingleRock_Mesh.obj"));
	if (magosSingleRockMesh) {
		magosSingleRockMesh->addTexture(string("Assets\\MagosTerrainBoard\\MagosRock_1k_Texture_DiffuseMap.bmp"), FIF_BMP);
		magosSingleRockMesh->addNormalMap(string("Assets\\MagosTerrainBoard\\MagosRock_1k_Texture_NormalMap.bmp"), FIF_BMP);
	}

	magosClusterRockMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\MagosClusterRock_Mesh.obj"));
	if (magosClusterRockMesh) {
		magosClusterRockMesh->addTexture(string("Assets\\MagosTerrainBoard\\MagosRockCluster_Texture_DiffuseMap.bmp"), FIF_BMP);
		magosClusterRockMesh ->addNormalMap(string("Assets\\MagosTerrainBoard\\MagosRockCluster_Texture_NormalMap.bmp"), FIF_BMP);
	}

	magosMapMesh = new AIMesh(string("Assets\\MagosTerrainBoard\\surface01.obj"));
	if (magosMapMesh) {
		magosMapMesh->addTexture(string("Assets\\MagosTerrainBoard\\magosMap_Texture.bmp"), FIF_BMP);
	}

	magosMapMesh2 = new AIMesh(string("Assets\\MagosTerrainBoard\\surface01.obj"));
	if (magosMapMesh2) {
		magosMapMesh2->addTexture(string("Assets\\MagosTerrainBoard\\magosMap2_Texture.bmp"), FIF_BMP);
	}

	magosBlackSandMesh = new AIMesh(string("Assets\\MagosGroundPlane\\MagosBlackSand_Mesh.obj"));
	if (magosBlackSandMesh) {
		magosBlackSandMesh->addTexture(string("Assets\\MagosGroundPlane\\MagosBlackSand_DiffuseMap.bmp"), FIF_BMP);
		magosBlackSandMesh->addNormalMap(string("Assets\\MagosGroundPlane\\MagosBlackSand_NormalMap.bmp"), FIF_BMP);
	}

	magosCrystalMesh = new AIMesh(string("Assets\\MagosGroundPlane\\magosCrystal_Mesh.obj"));
	if (magosCrystalMesh) {
		magosCrystalMesh->addTexture(string("Assets\\MagosGroundPlane\\Crystal_Texture.bmp"), FIF_BMP);
	}

	//
	//Background
	//
	 
	magosSpaceBGMesh = new AIMesh(string("Assets\\MagosBackGround\\surface01.obj"));
	if (magosSpaceBGMesh) {
		magosSpaceBGMesh->addTexture(string("Assets\\MagosBackGround\\MagosBG_Texture.bmp"), FIF_BMP);
	}


	// Load shaders
	basicShader = setupShaders(string("Assets\\Shaders\\basic_shader.vert"), string("Assets\\Shaders\\basic_shader.frag"));
	texPointLightShader = setupShaders(string("Assets\\Shaders\\texture-point.vert"), string("Assets\\Shaders\\texture-point.frag"));
	texDirLightShader = setupShaders(string("Assets\\Shaders\\texture-directional.vert"), string("Assets\\Shaders\\texture-directional.frag"));
	nMapDirLightShader = setupShaders(string("Assets\\Shaders\\nmap-directional.vert"), string("Assets\\Shaders\\nmap-directional.frag"));

	// Get uniform variable locations for setting values later during rendering
	basicShader_mvpMatrix = glGetUniformLocation(basicShader, "mvpMatrix");

	texDirLightShader_modelMatrix = glGetUniformLocation(texDirLightShader, "modelMatrix");
	texDirLightShader_viewMatrix = glGetUniformLocation(texDirLightShader, "viewMatrix");
	texDirLightShader_projMatrix = glGetUniformLocation(texDirLightShader, "projMatrix");
	texDirLightShader_texture = glGetUniformLocation(texDirLightShader, "texture");
	texDirLightShader_lightDirection = glGetUniformLocation(texDirLightShader, "lightDirection");
	texDirLightShader_lightColour = glGetUniformLocation(texDirLightShader, "lightColour");

	texPointLightShader_modelMatrix = glGetUniformLocation(texPointLightShader, "modelMatrix");
	texPointLightShader_viewMatrix = glGetUniformLocation(texPointLightShader, "viewMatrix");
	texPointLightShader_projMatrix = glGetUniformLocation(texPointLightShader, "projMatrix");
	texPointLightShader_texture = glGetUniformLocation(texPointLightShader, "texture");
	texPointLightShader_lightPosition = glGetUniformLocation(texPointLightShader, "lightPosition");
	texPointLightShader_lightColour = glGetUniformLocation(texPointLightShader, "lightColour");
	texPointLightShader_lightAttenuation = glGetUniformLocation(texPointLightShader, "lightAttenuation");

	nMapDirLightShader_modelMatrix = glGetUniformLocation(nMapDirLightShader, "modelMatrix");
	nMapDirLightShader_viewMatrix = glGetUniformLocation(nMapDirLightShader, "viewMatrix");
	nMapDirLightShader_projMatrix = glGetUniformLocation(nMapDirLightShader, "projMatrix");
	nMapDirLightShader_diffuseTexture = glGetUniformLocation(nMapDirLightShader, "diffuseTexture");
	nMapDirLightShader_normalMapTexture = glGetUniformLocation(nMapDirLightShader, "normalMapTexture");
	nMapDirLightShader_lightDirection = glGetUniformLocation(nMapDirLightShader, "lightDirection");
	nMapDirLightShader_lightColour = glGetUniformLocation(nMapDirLightShader, "lightColour");


	//
	// 2. Main loop
	// 

	while (!glfwWindowShouldClose(window)) {

		updateScene();
		renderScene();						// Render into the current buffer
		glfwSwapBuffers(window);			// Displays what was just rendered (using double buffering).

		glfwPollEvents();					// Use this version when animating as fast as possible
	
		// update window title
		char timingString[256];
		sprintf_s(timingString, 256, "CIS5013: Average fps: %.0f; Average spf: %f", gameClock->averageFPS(), gameClock->averageSPF() / 1000.0f);
		glfwSetWindowTitle(window, timingString);
	}

	glfwTerminate();

	if (gameClock) {

		gameClock->stop();
		gameClock->reportTimingData();
	}

	return 0;
}


// renderScene - function to render the current scene
void renderScene()
{
	//PopulusMagosLevel1();
	PopulusMagosLevel2();
}


// 
//PopulusMagos Level 1
//

void PopulusMagosLevel1 () {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform();


#pragma region Render all opaque objects with directional light

	//
	//Plug in the direction light shader
	//
	glUseProgram(texDirLightShader);

	glUniformMatrix4fv(texDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texDirLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(texDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));


	
	//Characters Direction Light

	
	if (starForgerMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), starForgerPos) * eulerAngleY<float>(glm::radians<float>(starForgerRotation));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		starForgerMesh->setupTextures();
		starForgerMesh->render();
	}
	

	//Buildings Direction Light

	

	if (astraOutpostMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-40.0f, 0.0f, -5.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraOutpostMesh->setupTextures();
		astraOutpostMesh->render();
	}

	if (astraOutpostMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-55.0f, 0.0f, 10.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraOutpostMesh->setupTextures();
		astraOutpostMesh->render();
	}


	if (astraOutpostMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-40.0f, 0.0f, 10.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraOutpostMesh->setupTextures();
		astraOutpostMesh->render();
	}
	
	


	//Terrain Direction Light
	if (magosBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(16.5f, -0.7f, 15.0f)) * glm::scale(identity<mat4>(), vec3(1.5f, 1.0f, 1.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosBoardMesh->setupTextures();
		magosBoardMesh->render();
	}

	if (magosMapBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-2.0f, -1.0f, 0.0f)) * glm::scale(identity<mat4>(), vec3(0.7f, 0.3f, 0.7f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosMapBoardMesh->setupTextures();
		magosMapBoardMesh->render();
	}

	if (magosShrineBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.5f, -0.7f, -10.0f)) * glm::scale(identity<mat4>(), vec3(1.0f, 0.5f, 1.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosShrineBoardMesh->setupTextures();
		magosShrineBoardMesh->render();
	}
	
	if (magosMapMesh2) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(45.0f, 0.2f, 0.0f)) * glm::scale(identity<mat4>(), vec3(15.0f, 0.5f, 15.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosMapMesh2->setupTextures();
		magosMapMesh2->render();
	}
	


	//
	//Crystal Mesh Direction Light
	//
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	if (magosCrystalMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, -11.0f, 2.5f)) * eulerAngleX<float>(glm::radians<float>(45.0f)) * glm::scale(identity<mat4>(), vec3(0.20f, 0.25f, 0.20f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);
		


		magosCrystalMesh->setupTextures();
		magosCrystalMesh->render();
	}

	//
	//Crystal Mesh Shrine Direction Light
	//
	if (magosCrystalMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, 0.0f, 65.0f)) * glm::scale(identity<mat4>(), vec3(0.42f, 0.47f, 0.42f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);



		magosCrystalMesh->setupTextures();
		magosCrystalMesh->render();
	}

	glDisable(GL_BLEND);

	


	//
	//Backgrounds Directional Light
	//

	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(20.0f, 0.2f, 60.0f))* eulerAngleX<float>(glm::radians<float>(-80.0f)) * glm::scale(identity<mat4>(), vec3(100.0f, 1.0f, 50.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}


	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(70.0f, 0.2f, 20.0f)) * eulerAngleZ<float>(glm::radians<float>(80.0f)) * glm::scale(identity<mat4>(), vec3(50.0f, 1.0f, 100.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}

	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(20.0f, -48.0f, 20.0f))  * glm::scale(identity<mat4>(), vec3(100.0f, 1.0f, 100.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}


	//  *** normal mapping ***  Render the normal mapped MagosGround
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	
	///Magos Ground Normal Mapped
	
	if (magosgroundMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(6.0f, -0.6f, 4.75f)) * glm::scale(identity<mat4>(), vec3(0.55f, 0.25f, 0.52f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosgroundMesh->setupTextures();
		magosgroundMesh->render();
	}

	///Magos Cluster Rock Normal Mapped Middle Map

	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, -1.1f, -7.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}

	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, -1.1f, -4.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}

	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, -1.1f, -2.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}


	///Magos Cluster Rock Normal Mapped Shrine Location
	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.0f, -1.1f, 5.0f)) * eulerAngleY<float>(glm::radians<float>(180.0f)) * glm::scale(identity<mat4>(), vec3(0.7f, 0.7f, 0.7f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}


	///Magos Black Sand Normal Mapped
	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-3.0f, 0.0f, 6.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.3f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-19.0f, -0.5f, 9.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(1.0f, 0.3f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

	///Magos Black Sand Shrine Normal Mapped
	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-9.0f, -0.5f, 52.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.8f, 0.8f, 0.8f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

#pragma endregion







#pragma region Render transparant objects

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


#pragma endregion






	//
	//  Render light sources
	//

	// Restore fixed-function
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	
	glBegin(GL_POINTS);

	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x *50.0f, directLight.direction.y *50.0f, directLight.direction.z * 0.0f);

	glColor3f(lights[0].colour.r, lights[0].colour.g, lights[0].colour.b);
	glVertex3f(lights[0].pos.x, lights[0].pos.y, lights[0].pos.z);
	
	glEnd();
}

//
//Populus Magos level 2
//

void PopulusMagosLevel2() {


	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform();


#pragma region Render all opaque objects with directional light

	//
	//Plug in the direction light shader
	//
	glUseProgram(texDirLightShader);

	glUniformMatrix4fv(texDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texDirLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(texDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));


	//
	//Characters Direction Light
	//

	if (starForgerMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), starForgerPos) * eulerAngleY<float>(glm::radians<float>(starForgerRotation));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		starForgerMesh->setupTextures();
		starForgerMesh->render();
	}

	/**/
	if (starWalkerMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(starWalkerPos)) * eulerAngleY<float>(glm::radians<float>(starWalkerRotation)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		starWalkerMesh->setupTextures();
		starWalkerMesh->render();
	}


	//
	//Buildings Direction Light
	//
	if (astraOutpostMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-65.0f, 0.0f, -5.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraOutpostMesh->setupTextures();
		astraOutpostMesh->render();
	}



	if (astraBrigadeMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(9.0f, -10.7f, 8.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraBrigadeMesh->setupTextures();
		astraBrigadeMesh->render();
	}


	//
	//Terrain  Direction Light
	//
	if (magosBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(16.5f, -0.7f, 15.0f)) * glm::scale(identity<mat4>(), vec3(1.5f, 1.0f, 1.5f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosBoardMesh->setupTextures();
		magosBoardMesh->render();
	}

	if (magosMapBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-2.0f, -1.0f, 0.0f)) * glm::scale(identity<mat4>(), vec3(0.7f, 0.3f, 0.7f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosMapBoardMesh->setupTextures();
		magosMapBoardMesh->render();
	}

	if (magosShrineBoardMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.5f, -0.7f, -10.0f)) * glm::scale(identity<mat4>(), vec3(1.0f, 0.5f, 1.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosShrineBoardMesh->setupTextures();
		magosShrineBoardMesh->render();
	}

	if (magosMapMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(45.0f, 0.2f, 0.0f)) * glm::scale(identity<mat4>(), vec3(15.0f, 0.5f, 15.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosMapMesh->setupTextures();
		magosMapMesh->render();
	}



	//
	//Crystal Mesh  Direction Light
	//

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	if (magosCrystalMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.f, 1.0f, 9.5f)) * glm::scale(identity<mat4>(), vec3(0.20f, 0.25f, 0.20f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);



		magosCrystalMesh->setupTextures();
		magosCrystalMesh->render();
	}


	//Crystal Mesh Shrine Direction Light
	if (magosCrystalMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(5.0f, 0.0f, 65.0f)) * glm::scale(identity<mat4>(), vec3(0.42f, 0.47f, 0.42f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);



		magosCrystalMesh->setupTextures();
		magosCrystalMesh->render();
	}

	glDisable(GL_BLEND);




	//
	//Backgrounds Direction Light
	//

	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(20.0f, 0.2f, 60.0f)) * eulerAngleX<float>(glm::radians<float>(-80.0f)) * glm::scale(identity<mat4>(), vec3(100.0f, 1.0f, 50.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}


	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(70.0f, 0.2f, 20.0f)) * eulerAngleZ<float>(glm::radians<float>(80.0f)) * glm::scale(identity<mat4>(), vec3(50.0f, 1.0f, 100.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}

	if (magosSpaceBGMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(20.0f, -48.0f, 20.0f)) * glm::scale(identity<mat4>(), vec3(100.0f, 1.0f, 100.0f));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		magosSpaceBGMesh->setupTextures();
		magosSpaceBGMesh->render();
	}


	//  *** normal mapping ***  Render the normal mapped MagosGround
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	//
	///Magos Ground Normal Mapped
	//
	if (magosgroundMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(6.0f, -0.6f, 4.75f)) * glm::scale(identity<mat4>(), vec3(0.55f, 0.25f, 0.52f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosgroundMesh->setupTextures();
		magosgroundMesh->render();
	}

	//
	///Magos Ground Normal Mapped
	//
	if (magosSingleRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(15.0f, -4.0f, -8.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 1.0f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosSingleRockMesh->setupTextures();
		magosSingleRockMesh->render();
	}

	if (magosSingleRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(15.0f, -8.0f, -9.5f)) * glm::scale(identity<mat4>(), vec3(0.5f, 2.0f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosSingleRockMesh->setupTextures();
		magosSingleRockMesh->render();
	}

	if (magosSingleRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(13.7f, -6.0f, -9.5f)) * glm::scale(identity<mat4>(), vec3(0.5f, 1.5f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosSingleRockMesh->setupTextures();
		magosSingleRockMesh->render();
	}

	//
	///Magos Cluster Rock Normal Mapped Middle Map
	//
	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(15.0f, -1.1f, -2.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}

	///Magos Cluster Rock Normal Mapped Shrine Location
	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(0.0f, -1.1f, 5.0f)) * eulerAngleY<float>(glm::radians<float>(180.0f)) * glm::scale(identity<mat4>(), vec3(0.7f, 0.7f, 0.7f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}


	///Magos Black Sand Normal Mapped
	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-9.0f, -0.5f, 6.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.3f, 0.5f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

	///Magos Black Sand Shrine Normal Mapped
	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-9.0f, -0.5f, 52.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.8f, 0.8f, 0.8f));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

#pragma endregion



	// Enable additive blending for ***subsequent*** light sources!!!
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);



#pragma region Render all opaque objects with point light
//
// 	   Point Light
//
	glUseProgram(texPointLightShader);

	glUniformMatrix4fv(texPointLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texPointLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texPointLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texPointLightShader_lightPosition, 1, (GLfloat*)&(lights[0].pos));
	glUniform3fv(texPointLightShader_lightColour, 1, (GLfloat*)&(lights[0].colour));
	glUniform3fv(texPointLightShader_lightAttenuation, 1, (GLfloat*)&(lights[0].attenuation));

	//Characters with Point Light

	if (starForgerMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), starForgerPos) * eulerAngleY<float>(glm::radians<float>(starForgerRotation));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		starForgerMesh->setupTextures();
		starForgerMesh->render();
	}

	if (starWalkerMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(starWalkerPos)) * eulerAngleY<float>(glm::radians<float>(starWalkerRotation)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		starWalkerMesh->setupTextures();
		starWalkerMesh->render();
	}


	//Ground Scape with point light
	if (magosgroundMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(1.0f, -0.6f, 0.0f)) * glm::scale(identity<mat4>(), vec3(0.50f, 0.25f, 0.50f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosgroundMesh->setupTextures();
		magosgroundMesh->render();
	}

	if (magosBlackSandMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-9.0f, -0.5f, 6.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.3f, 0.5f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosBlackSandMesh->setupTextures();
		magosBlackSandMesh->render();
	}

	if (magosClusterRockMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(15.0f, -1.1f, -2.0f)) * eulerAngleY<float>(glm::radians<float>(90.0f)) * glm::scale(identity<mat4>(), vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		magosClusterRockMesh->setupTextures();
		magosClusterRockMesh->render();
	}

	//Astra buildings with point light

	if (astraBrigadeMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(9.0f, -10.7f, 8.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraBrigadeMesh->setupTextures();
		astraBrigadeMesh->render();
	}

	if (astraOutpostMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), vec3(-65.0f, 0.0f, -5.0f)) * eulerAngleY<float>(glm::radians<float>(-90.0f)) * glm::scale(identity<mat4>(), vec3(0.5f, 0.5f, 0.5f));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);


		astraOutpostMesh->setupTextures();
		astraOutpostMesh->render();
	}


#pragma endregion


#pragma region Render transparant objects

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


#pragma endregion


	//
	//  Render light sources
	//

	// Restore fixed-function
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);

	glBegin(GL_POINTS);

	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x * 50.0f, directLight.direction.y * 50.0f, directLight.direction.z * 0.0f);

	glColor3f(lights[0].colour.r, lights[0].colour.g, lights[0].colour.b);
	glVertex3f(lights[0].pos.x, lights[0].pos.y, lights[0].pos.z);

	glEnd();




}


// Function called to animate elements in the scene
void updateScene() {

	float tDelta = 0.0f;

	if (gameClock) {

		gameClock->tick();
		tDelta = (float)gameClock->gameTimeDelta();
	}



	// update main light source

	
	if (rotateDirectionalLight) {

		directLightTheta += glm::radians(20.0f) * tDelta;
		directLight.direction = vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f);
	}
	

	//
	// Handle movement based on user input
	//

	float moveSpeed = 3.0f; // movement displacement per second
	float rotateSpeed = 90.0f; // degrees rotation per second

	

		if (forwardPressed) {

			mat4 R = eulerAngleY<float>(glm::radians<float>(starForgerRotation)); // local coord space / basis vectors - move along z
			float dPos = moveSpeed * tDelta; // calc movement based on time elapsed
			starForgerPos += vec3(R[2].x * dPos, R[2].y * dPos, R[2].z * dPos); // add displacement to position vector
		}
		else if (backPressed) {

			mat4 R = eulerAngleY<float>(glm::radians<float>(starForgerRotation)); // local coord space / basis vectors - move along z
			float dPos = -moveSpeed * tDelta; // calc movement based on time elapsed
			starForgerPos += vec3(R[2].x * dPos, R[2].y * dPos, R[2].z * dPos); // add displacement to position vector
		}

		if (rotateLeftPressed) {

			starForgerRotation += rotateSpeed * tDelta;
		}
		else if (rotateRightPressed) {

			starForgerRotation -= rotateSpeed * tDelta;
		}
	}



#pragma region Event handler functions

// Function to call when window resized
void resizeWindow(GLFWwindow* window, int width, int height)
{
	if (mainCamera) {

		mainCamera->setAspect((float)width / (float)height);
	}

	// Update viewport to cover the entire window
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;
}


// Function to call to handle keyboard input
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {

		// check which key was pressed...
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			
			case GLFW_KEY_W:
				forwardPressed = true;
				break;

			case GLFW_KEY_S:
				backPressed = true;
				break;

			case GLFW_KEY_A:
				rotateLeftPressed = true;
				break;

			case GLFW_KEY_D:
				rotateRightPressed = true;
				break;

			case GLFW_KEY_SPACE:
				rotateDirectionalLight = !rotateDirectionalLight;
				break;


		
			default:
			{
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		// handle key release events
		switch (key)
		{
			case GLFW_KEY_W:
				forwardPressed = false;
				break;

			case GLFW_KEY_S:
				backPressed = false;
				break;

			case GLFW_KEY_A:
				rotateLeftPressed = false;
				break;

			case GLFW_KEY_D:
				rotateRightPressed = false;
				break;

			default:
			{
			}
		}
	}
}


void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos) {

	if (mouseDown) {

		float dx = float(xpos - prevMouseX);
		float dy = float(ypos - prevMouseY);

		if (mainCamera)
			mainCamera->rotateCamera(-dy, -dx);

		prevMouseX = xpos;
		prevMouseY = ypos;
	}

}

void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT) {

		if (action == GLFW_PRESS) {

			mouseDown = true;
			glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
		}
		else if (action == GLFW_RELEASE) {

			mouseDown = false;
		}
	}
}

void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {

	if (mainCamera) {

		if (yoffset < 0.0)
			mainCamera->scaleRadius(1.1f);
		else if (yoffset > 0.0)
			mainCamera->scaleRadius(0.9f);
	}
}

void mouseEnterHandler(GLFWwindow* window, int entered) {
}

#pragma endregion