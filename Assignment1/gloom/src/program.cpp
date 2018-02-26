// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "utilities.hpp"
//global
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

// Function for seting up VAO
GLuint setupVAO(float vertices[], unsigned int vertlength, unsigned int indices[], unsigned int indiceslength, float colours[], unsigned int colourlength);
void updateMVP();
void cameraMovement(GLFWwindow* window);

GLuint setupTexture(std::string filepath);

//Function to draw the scene
void drawScene(GLsizei element, unsigned int vaoID, unsigned int textureID);
//motion is the position to the carmera
glm::vec3 motion = glm::vec3(0, 0, 0);
glm::mat4x4 projection = glm::perspective(glm::radians(120.0), 4.0 / 3.0, 1.0, 100.0);
glm::mat4 Model = glm::mat4(1.0f);
glm::mat4x4 Scale = glm::mat4(1.0f);
glm::mat4 Rotation = glm::mat4(1.0f);
//sat to rotate around x-axis by defaulte
glm::vec3 Rotationaxis = glm::vec3(1, 0, 0);
float angle = 0;
//the tranformation matrix
glm::mat4 MVP;



void runProgram(GLFWwindow* window)
{
	//local variables
	unsigned int vaoID;
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Set default colour after clearing the colour buffer
	glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
	// Set up your scene here (create Vertex Array Objects, etc.)
	Gloom::Shader shader;
	shader.makeBasicShader("../gloom/shaders/simple.vert", "../gloom/shaders/simple.frag");
	shader.activate();


	// vertices for one triangle
	float verticesOneTriangle[] = {
		-0.6, -0.3, -1.2, //0
		0, -0.3, -1.2, //1
		0, 0.3, -1.2,
		-0.6, 0.3, -1.2
	};
	//indices for one triangle
	unsigned int indicesOneTriangle[] = {
		0, 1, 2,
		0, 2, 3
	};

	float Texture[] = {
		0, 0.5, //0
		0.5, 0.1,  //1
		1, 0.5,
		0.5, 0.9
	};
	unsigned int textureID;
	//vaoID = setupVAO(vertices, 15 * 3, indices, 15, colours, 15 * 4);
	vaoID = setupVAO(verticesOneTriangle, 6 * 3, indicesOneTriangle, 6, Texture, 2 * 4);
	textureID = setupTexture("../diamond.png");
	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//vaoID = setupVAO(verticesOneTriangle, 9, indicesOneTriangle, 3, colour, 3 * 4);
		// Draw your scene here
		glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(vaoID);
		drawScene(6, vaoID, textureID);
		updateMVP();
		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);
		cameraMovement(window);
		// Flip buffers
		glfwSwapBuffers(window);
	}
}


GLuint setupVAO(float* vertices, unsigned int vertlength, unsigned int* indices, unsigned int indiceslength,
	float* texture, unsigned int texturelength) {
	GLuint vaoID = 0;
	GLuint vboID = 0;
	GLuint iboID = 1;
	GLuint cboID = 2;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	//VBO
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertlength, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//IBO
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indiceslength, indices, GL_STATIC_DRAW);

	//CBO
	glGenBuffers(1, &cboID);
	glBindBuffer(GL_ARRAY_BUFFER, cboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*texturelength, texture, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	printf("%s\n","Test" );
	return vaoID;
}

GLuint setupTexture(std::string filepath) {
	PNGImage image;
	image = loadPNGFile(filepath);
	GLuint textureID = 1;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//std::cout << image.pixels[0]+ "\n" << std::endl;
	printf("%d %d \n", &image.pixels[0],image.pixels);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.pixels[0]);
	glBindTextureUnit(5,textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return textureID;
}

void drawScene(GLsizei element, unsigned int vaoID, unsigned int textureID) {
	//Draw the triangles
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vaoID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindTextureUnit(5,textureID);

	glDrawElements(GL_TRIANGLES, element, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	updateMVP();
}
//g
void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void updateMVP() {
	glm::mat4 View = glm::translate(-motion)* Rotation* Scale;
	MVP = projection * View * Model;
}

void cameraMovement(GLFWwindow* window) {
	float speed = 0.01;
	//use a to move forward on the x-axias and s to move backwards
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		motion.x += speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		motion.x -= speed;
	}
	//use n to move forward on y-axis and m to move backwards
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		motion.y += speed;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		motion.y -= speed;
	}
	// Use i to zoom and o to zoom out
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		motion.z += speed;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		motion.z -= speed;
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		Scale = glm::scale(glm::vec3(Scale[0][0] + speed, Scale[0][0] + speed, Scale[0][0] + speed));
	}
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
		Scale = glm::scale(glm::vec3(Scale[0][0] - speed, Scale[0][0] - speed, Scale[0][0] - speed));
	}
	//rotion forword on the x-axis
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(0, 1, 0);
		angle += speed;
	}
	//rotion backwards on the x-axis
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(0, 1, 0);
		angle -= speed;
	}
	//rotion up on the y-axis
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(1, 0, 0);
		angle += speed;
	}
	//rotion down on the y-axis
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(1, 0, 0);
		angle -= speed;
	}
	//rotion forword on the z-axis
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(0, 0, 1);
		angle += speed;
	}
	//rotion backwards on the z-axis
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		Rotationaxis = glm::vec3(0, 0, 1);
		angle -= speed;
	}
	Rotation = glm::rotate(angle, Rotationaxis);
}
