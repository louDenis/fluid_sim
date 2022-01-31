#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PerlinNoise.h"
#include "fluid.h"
#include "fluid_utils.h"

Fluid* fluid;


//------------------------------SHADERS------------------------------
const char* vertexShaderSource =
"#version 330 core \n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTex;\n"

"out vec2 aTexture;\n"

"void main() { \n"
"gl_Position = vec4(aPos, 1.0f);\n"
"aTexture = aTex;\n"
"}\n \0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 aTexture;\n"
"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
"	FragColor = texture(ourTexture, aTexture);\n"
"}\n \0";
//-------------------------------------------------------------------


void framebuffer_size_callback(GLFWwindow* win, int width, int height);
void processInput(GLFWwindow* window);
std::string readFile(const char* path);

int width = SCALE*N, height = SCALE*N;

float vertices[] = {
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f
};

unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
};

uint8_t *texture;
uint8_t *brush;
double x, y;

int main(int argc, char **argv) {

	//FLUID DECLARATION
	fluid= new Fluid(0.2, 0, 0.0000001);

	glfwInit();
	GLFWwindow *window = glfwCreateWindow(width, height, "aled", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);


	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	

	//GPU buffers
	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texture initialisation
	texture = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
	PerlinNoise pn;
	for (int line = 0; line < height; line++) {
		for (int col = 0; col < width; col++) {
			double x = (double)col / (double)height;
			double y = (double)line / (double)width;
			double noise = pn.noise(1000 * x, 1000 * y, 0.8f) * 255;
			//std::cout << noise << std::endl;
			texture[(line * width * 3) + (col * 3) + 0] = (uint8_t)noise;
			texture[(line * width * 3) + (col * 3) + 1] = (uint8_t)noise;
			texture[(line * width * 3) + (col * 3) + 2] = (uint8_t)noise;
		}
	}


	int brush_width = SCALE;
	int brush_height = SCALE;

	brush = (uint8_t*)malloc(brush_width * brush_height * 3 * sizeof(uint8_t));
	


	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, textureID);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
		fluid->step();

		for (int line = 0; line < brush_height; line++) {
			for (int col = 0; col < brush_width; col++) {
				brush[(line * brush_width * 3) + (col * 3) + 0] = 255; //(uint8_t)fluid->getDensity(x,y);
				brush[(line * brush_width * 3) + (col * 3) + 1] = 0;//(uint8_t)fluid->getDensity(x, y);
				brush[(line * brush_width * 3) + (col * 3) + 2] = 0;//(uint8_t)fluid->getDensity(x, y);
			}
		}

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				int h = i * SCALE;
				int w = j * SCALE;
				
				for (int line = 0; line < brush_height; line++) {
					for (int col = 0; col < brush_width; col++) {
						brush[(line * brush_width * 3) + (col * 3) + 0] = (uint8_t)fluid->getDensity(i, j);
						brush[(line * brush_width * 3) + (col * 3) + 1] = (uint8_t)fluid->getDensity(i, j);
						brush[(line * brush_width * 3) + (col * 3) + 2] = (uint8_t)fluid->getDensity(i, j);
					}
				}
				
				glTexSubImage2D(GL_TEXTURE_2D, 0, w, height - h, 15, 10, GL_RGB, GL_UNSIGNED_BYTE, brush);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
	}

	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* win, int width, int height){
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	int previousX = x;
	int previousY = y;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwGetCursorPos(window,&x, &y);
		std::cout << x << "  " << y << std::endl;
		fluid->addDensity(x/SCALE, y/SCALE, 100);

		float amtX = (float)x - previousX;
		float amtY = (float)y - previousY;
		fluid->addVelocity(x / SCALE, y / SCALE, amtX, amtY);


		//glTexSubImage2D(GL_TEXTURE_2D, 0, x, height - y, 15, 10, GL_RGB, GL_UNSIGNED_BYTE, brush);
		//glGenerateMipmap(GL_TEXTURE_2D);
		

	}
}


std::string readFile(const char* path) {
	std::string fileContent;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file.open(path);
		std::stringstream fileStream;
		fileStream << file.rdbuf();
		file.close();
		fileContent = fileStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR reading file" << std::endl;
	}
	return fileContent;
}