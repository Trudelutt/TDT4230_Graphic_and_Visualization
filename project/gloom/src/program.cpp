// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "utilities.hpp"
#include "OBJLoader.h"
#include "sceneGraph.hpp"
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

GLuint setupVaoSquare(float vertices[], unsigned int vertlength, unsigned int indices[],
	unsigned int indiceslength, float textureUV[], unsigned int textureUVlenght, unsigned int pos);

void updateMVP();
void updateMP();
void updateMVPportal(glm::mat4 portalSrcPos, glm::mat4 portalDistPos);
void cameraMovement(GLFWwindow* window);
GLuint setupTexture(std::string filepath,unsigned int id);
GLuint setupDepthTexture();

//Function to draw the scene
void drawScene(GLsizei element, unsigned int vaoID);
//motion is position to the carmera

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -5.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);




//glm::mat4 projection = glm::frustum(-40.0f, 40.0f, -5.0f, 50.0f, 40.0f, -40.0f);
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
	Gloom::Shader wallshader;
	wallshader.makeBasicShader("../gloom/shaders/wall.vert", "../gloom/shaders/wall.frag");
	Gloom::Shader rabbitshader;
	rabbitshader.makeBasicShader("../gloom/shaders/rabbit.vert", "../gloom/shaders/rabbit.frag");
	Gloom::Shader portalShader;
	portalShader.makeBasicShader("../gloom/shaders/portal.vert", "../gloom/shaders/portal.frag");



	float verticesSquare[] = {
		-40.0, 0.0, 40.0, //0
		40.0, 0.0, 40.0, //1
		40.0, 0.0, -40.0,
		-40.0, 0.0, -40.0
	};

	float ceilingverticesSquare[] = {
		40.0, 40.0, 40.0, //0
		-40.0, 40.0, 40.0, //1
		-40.0, 40.0, -40.0,
		40.0, 40.0, -40.0
	};

	float wallverticesSquare[] = {
		-40.0, 0.0, -40.0, //0
		40.0, 0.0, -40.0, //1
		40.0, 50.0, -40.0,
		-40.0, 50.0, -40.0
	};

	float leftwallverticesSquare[] = {
		-40.0, 0.0, -40.0, //0
		-40.0, 50.0, -40.0, //1
		-40.0, 50.0, 40.0,
		-40.0, 0.0, 40.0
	};

	float rightwallverticesSquare[] = {
		40.0, 0.0, -40.0, //0
		40.0, 0.0, 40.0, //1
		40.0, 50.0, 40.0,
		40.0, 50.0, -40.0
	};

	float backwallverticesSquare[] = {
		40.0, 0.0, 40.0, //0
		-40.0, 0.0, 40.0, //1
		-40.0, 50.0, 40.0,
		40.0, 50.0, 40.0
	};

	float textureUV[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0

	};
	//indices for one triangle
	unsigned int indicesSquare[] = {
		0, 1, 2,
		0, 2, 3
	};

	float portal_vertices[] = {
		1, 1, 0,
		10, 1, 0, 
		1,  10, 0, 
		10,  10, 0
	};

	unsigned int portal_indices[] = {
		0,1,2, 2,1,3,
	};

	glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));


	unsigned int textureID = setupTexture("../assets/floor.png", 1);
	unsigned int walltextureID = setupTexture("../assets/wall.png",2);

	//VAO
	unsigned int vaosquareID = setupVaoSquare(verticesSquare, 3*4, indicesSquare, 6, textureUV, 8,0);
	unsigned int ceilingvaosquareID = setupVaoSquare(ceilingverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	unsigned int wallvaosquareID = setupVaoSquare(wallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8,0);
	unsigned int leftwallvaosquareID = setupVaoSquare(leftwallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	unsigned int rightwallvaosquareID = setupVaoSquare(rightwallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	unsigned int backwallvaosquareID = setupVaoSquare(backwallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	Mesh rabbitobj = loadOBJ("../Rabbit/Rabbit.obj");
	unsigned int rabbitvaoID = setupVAO(rabbitobj.vertices, rabbitobj.vertexCount, rabbitobj.indices, rabbitobj.indexCount, rabbitobj.textureCoordinates, rabbitobj.normals);
	unsigned int rabbittextureID = setupTexture("../Rabbit/Rabbit_D.png", 3);
	unsigned int amibientOcculationid = setupTexture("../Rabbit/Rabbit_S.png", 4);
	unsigned int normalmapid = setupTexture("../Rabbit/Rabbit_N.png", 5);

	unsigned int portalid = setupVaoSquare(wallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	unsigned int portal2id = setupVaoSquare(wallverticesSquare, 3 * 4, indicesSquare, 6, textureUV, 8, 0);
	GLuint fbo = setupDepthTexture();
	float timeElapsed = 0;
	//uniform_name = "fbo_texture";

	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		timeElapsed += getTimeDeltaSeconds();
		glUniform1f(9, timeElapsed);

		//glEnable(GL_STENCIL_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		



		
		
	
		//glutSwapBuffers();
		
		//Floor and ceiling
		shader.activate();
		glUniform3fv(7,1,  glm::value_ptr(cameraFront));
		glBindVertexArray(vaosquareID);
		drawScene(12, vaosquareID);
		updateMP();
		glBindVertexArray(ceilingvaosquareID);
		drawScene(12, ceilingvaosquareID);
		updateMP();

		//wall
		wallshader.activate();
	
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(Model));
		//glUniform3fv(7, 1, glm::value_ptr(-motion));
		glBindVertexArray(wallvaosquareID);
		drawScene(12, wallvaosquareID);
		glBindVertexArray(leftwallvaosquareID);
		drawScene(12, leftwallvaosquareID);
		glBindVertexArray(rightwallvaosquareID);
		drawScene(12, rightwallvaosquareID);
		glBindVertexArray(backwallvaosquareID);
		drawScene(12, backwallvaosquareID);
		updateMP();

		//Rabbit
		rabbitshader.activate();
		//glUniform3fv(7, 1, glm::value_ptr(-motion));

		glBindVertexArray(rabbitvaoID);
		drawScene(rabbitobj.vertexCount, rabbitvaoID);
		updateMVP();
		portalShader.activate();


		glUniform3fv(7, 1, glm::value_ptr(cameraFront));
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT);
		

		// Clear colour and depth buffers
		//fbo = setupDepthTexture();
		glBindVertexArray(portalid);
		drawScene(12, portalid);
		//updateMVPportal(glm::translate(glm::mat4(1), glm::vec3(0, 1, 10)), glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0))* glm::translate(glm::mat4(1), glm::vec3(0, 1.2, 10)));


		glBindVertexArray(portal2id);
		drawScene(12, portal2id);
		//updateMVPportal(glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0))* glm::translate(glm::mat4(1), glm::vec3(0, 1.2, 10)), glm::translate(glm::mat4(1), glm::vec3(0, 1, 10)));

		//glUniform3fv(7, 1, glm::value_ptr(-motion));

		//glClear(GL_DEPTH_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT);
		glBindVertexArray(portalid);
		drawScene(12, portalid);
		updateMVPportal(glm::translate(glm::mat4(1), glm::vec3(0, 1, 10)), glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0))* glm::translate(glm::mat4(1), glm::vec3(0, 1.2, 10)));
		fbo = setupDepthTexture();

		glBindVertexArray(portal2id);
		drawScene(12, portal2id);
		updateMVPportal(glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0))* glm::translate(glm::mat4(1), glm::vec3(0, 1.2, 10)), glm::translate(glm::mat4(1), glm::vec3(0, 1, 10)));
		glDisable(GL_STENCIL_TEST);
		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);
		cameraMovement(window);
		// Flip buffers
		glfwSwapBuffers(window);
	

	}
}

GLuint setupVaoSquare(float vertices[], unsigned int vertlength, unsigned int indices[],
	unsigned int indiceslength, float textureUV[], unsigned int textureUVlenght, unsigned int pos) {
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	//VBO
	GLuint vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertlength, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(pos);

	//IBO
	GLuint iboID;
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indiceslength, indices, GL_STATIC_DRAW);

	GLuint uvID;
	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* textureUVlenght, textureUV, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	return vaoID;
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

	for (unsigned int i = 0; i < vertlength; i += 3)
	{
		float3 tangent;

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
	
		//wil push all the three vertices on the tangent and biTangent arrays
		for (int j = 0; j < 3; j++)
		{
			tangents.push_back(tangent);
		
		}
	}

	float3 *tangentsBuffer = new float3[tangents.size()];
	std::copy(tangents.begin(), tangents.end(), tangentsBuffer);


	GLuint tangentid;
	glGenBuffers(1, &tangentid);
	glBindBuffer(GL_ARRAY_BUFFER, tangentid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)*tangents.size(), tangentsBuffer, GL_STATIC_DRAW);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
	glBindTextureUnit(id,textureID);
	// enable mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}

GLuint setupDepthTexture() {

	GLuint fbo_texture;

	GLuint textureID = 1;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, windowWidth, windowHeight, 0);
	glBindTextureUnit(15, textureID);
	// enable mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}

void drawScene(GLsizei element, unsigned int vaoID) {
	//Draw the triangles
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(11);


	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, element, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(vaoID);
	
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	updateMVP();
}
//
void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void updateMP() {
	glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
	glm::mat4 MP = projection*View*glm::mat4(1.0f);
	glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(MP));
}

void updateMVP() {
	//find out where the object is placed
	glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
	Model =glm::translate(cameraFront)* Scale * Rotation;
	MVP = projection * View * Model;
	glUniformMatrix4fv(15, 1, GL_FALSE, glm::value_ptr(MVP));
}

void updateMVPportal(glm::mat4 portalSrcPos, glm::mat4 portalDistPos) {
	glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
	glm::mat4 portalModel =  glm::scale(glm::vec3(0.1)) * portalSrcPos;
	glm::mat4 mv = View * portalModel;

	glm::mat4 portal_cam = mv * glm::rotate(glm::mat4(1.0), glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0))* glm::inverse(portalDistPos);
	MVP = projection * portal_cam;
	glUniformMatrix4fv(8, 1, GL_FALSE, glm::value_ptr(MVP));
}


void cameraMovement(GLFWwindow* window) {
	float speed = 0.01;
	//use a to move forward on the x-axias and s to move backwards
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += speed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= speed * cameraFront;
	}
	//use n to move forward on y-axis and m to move backwards
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, glm::vec3(0,1,0))) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0))) * speed;
	}
	// Use i to zoom and o to zoom out
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		cameraPos.x += speed;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		cameraPos.x -= speed;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		cameraPos.y += speed;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		cameraPos.y -= speed;
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



