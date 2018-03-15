// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "utilities.hpp"
#include "OBJLoader.h"
//global
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <chrono>




// Function for seting up VAO
GLuint setupVAO(float3* vertices, unsigned int vertlength, unsigned int* indices,
	 unsigned int indiceslength, float2* textureCoordinates, float3* normals);

GLuint calcTargetandBitangents(float3* vertices, unsigned int vertlength, unsigned int* indices,
	unsigned int indiceslength, float2* textureCoordinates, float3* normals);

void updateMVP();
void cameraMovement(GLFWwindow* window);
GLuint setupTexture(std::string filepath,unsigned int id);

//Function to draw the scene
void drawScene(GLsizei element, unsigned int vaoID, unsigned int textureID, unsigned int ambientTexture);
//motion is position to the carmera

glm::vec3 motion = glm::vec3(-2.0f, 1.0f, -8.0f);
glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 1.0f);
glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)windowWidth/(float)windowHeight, 0.1f, 1000.0f);
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



	unsigned int textureID;
	Mesh obj = loadOBJ("../assets/MFEP_Rock_3.obj");
	vaoID = setupVAO(obj.vertices, obj.vertexCount, obj.indices, obj.indexCount, obj.textureCoordinates, obj.normals);
	textureID = setupTexture("../assets/MFEP_Rock_3_DefaultMaterial_AlbedoTransparency.png",5);
	unsigned int animationID = setupTexture("../assets/animation.png", 11);
	unsigned int amibientOcculationid = setupTexture("../assets/MFEP_Rock_3_DefaultMaterial_AmbientOcclusion.png",13);
	unsigned int normalmapid = setupTexture("../assets/MFEP_Rock_3_DefaultMaterial_Normal.png", 14);

	glBindTextureUnit(5, textureID);
	glBindTextureUnit(11, animationID);
	glBindTextureUnit(13, amibientOcculationid);
	glBindTextureUnit(14, normalmapid);

	float timeElapsed = 0;
	glUniform1f(10, obj.vertexCount);
	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		timeElapsed += getTimeDeltaSeconds();
		glUniform1f(9, timeElapsed);
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draw your scene here
		glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(Model));
		glUniform3fv(7,1,  glm::value_ptr(-motion));


		glBindVertexArray(vaoID);
		drawScene(obj.vertexCount, vaoID, textureID, amibientOcculationid);
		updateMVP();
		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);
		cameraMovement(window);
		// Flip buffers
		glfwSwapBuffers(window);
	}
}


GLuint setupVAO(float3* vertices, unsigned int vertlength, unsigned int* indices, unsigned int indiceslength,
	float2* textureCoordinates, float3* normals) {
	GLuint vaoID;
	GLuint vboID;
	GLuint iboID;
	GLuint cboID;
	GLuint normalbuffer;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	//VBO
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertlength , vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//IBO
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indiceslength, indices, GL_STATIC_DRAW);

	//texturemaping
	glGenBuffers(1, &cboID);
	glBindBuffer(GL_ARRAY_BUFFER, cboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2)*vertlength, textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//normalbuffer
	glGenBuffers(1,&normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)*vertlength, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, 0, (void*)0);

	std::vector<float3> tangents;
	std::vector<float3> biTangents;

	for (unsigned int i = 0; i < vertlength; i += 3)
	{
		float3 tangent;
		float3 biTangent;

		float3 edge1 = float3(vertices[i + 1].x- vertices[i].x, vertices[i + 1].y - vertices[i].y, vertices[i + 1].z - vertices[i].z);
		float3 edge2 = float3(vertices[i + 2].x - vertices[i].x, vertices[i + 2].y - vertices[i].y, vertices[i + 2].z - vertices[i].z);
		float2 deltaUV1 = float2(textureCoordinates[i + 1].x - textureCoordinates[i].x, textureCoordinates[i + 1].y - textureCoordinates[i].y);
		float2 deltaUV2 = float2(textureCoordinates[i + 2].x - textureCoordinates[i].x, textureCoordinates[i + 2].y - textureCoordinates[i].y);

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		glm::vec3 tangentNormalized = glm::normalize(glm::vec3(tangent.x, tangent.y, tangent.z));
		tangent = float3(tangentNormalized.x, tangentNormalized.y, tangentNormalized.z);

		biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		glm::vec3 biTangentNormalized = glm::normalize(glm::vec3(biTangent.x, biTangent.y, biTangent.z));
		biTangent = float3(biTangentNormalized.x, biTangentNormalized.y, biTangentNormalized.z);
		//wil push all the three vertices on the tangent and biTangent arrays
		for (int j = 0; j < 3; j++)
		{
			tangents.push_back(tangent);
			biTangents.push_back(biTangent);
		}
	}

	float3 *tangentsBuffer = new float3[tangents.size()];
	std::copy(tangents.begin(), tangents.end(), tangentsBuffer);

	float3 *biTangentsBuffer = new float3[biTangents.size()];
	std::copy(biTangents.begin(), biTangents.end(), biTangentsBuffer);


	GLuint tangentid;
	glGenBuffers(1, &tangentid);
	glBindBuffer(GL_ARRAY_BUFFER, tangentid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)*tangents.size(), tangentsBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint bitangentid;
	glGenBuffers(1, &bitangentid);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)*biTangents.size(), biTangentsBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	return vaoID;
}

GLuint setupTexture(std::string filepath, unsigned int id) {
	PNGImage image;
	image = loadPNGFile(filepath);
	GLuint textureID = 1;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.pixels[0]);
	//glBindTextureUnit(id,textureID);
	// Use this if not mipmaping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// enable mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}


void drawScene(GLsizei element, unsigned int vaoID, unsigned int textureID, unsigned int ambienttexture) {
	//Draw the triangles
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, element, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(7);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(2);
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
	//find out where the object is placed
	glm::mat4 View = glm::lookAt(motion, glm::vec3(0, 0, 0)-motion,glm::vec3(0,1,0));
	glUniformMatrix4fv(8, 1, GL_FALSE, glm::value_ptr(View));
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
