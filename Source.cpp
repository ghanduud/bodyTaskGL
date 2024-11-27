
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <cmath>


using namespace std;
using namespace glm;

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, EBO, shaderProgram, basicProgramId;
float rotationAngle = 0.0f;

// Cube vertices (positions + colors)
GLfloat cubeVertices[] = {
	// Positions           // Colors
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,   0.2f, 0.0f, 0.4f
};

// Cube indices for drawing with elements
GLuint cubeIndices[] = {
	0, 1, 2, 2, 3, 0, // Back face
	4, 5, 6, 6, 7, 4, // Front face
	0, 1, 5, 5, 4, 0, // Bottom face
	2, 3, 7, 7, 6, 2, // Top face
	0, 4, 7, 7, 3, 0, // Left face
	1, 5, 6, 6, 2, 1  // Right face
};


GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);


// Function to compile a shader
void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_namering, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_namering);
	glUseProgram(programId);
}

int Init()
{
	GLuint basicProgramId;
	CompileShader("VS.glsl", "FS.glsl", basicProgramId);  // Compile and link shaders
	shaderProgram = basicProgramId;  // Set shader program

	// No need to manually attach shaders here, it's already done in CompileShader()

	// Generate and bind VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Bind VBO and EBO buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// Set vertex attributes (positions and colors)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);  // Position attribute
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));  // Color attribute
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);  // Enable depth test for 3D rendering

	return 0;  // Return success
}

// Draw a cube with transformations
void DrawCube(mat4 model, GLuint modelLoc)
{
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void RenderBody(GLuint modelLoc)
{
	// Base rotation and scaling
	float scaleFactor = 0.02f; // Reduce body size to 1/4 of its original size
	mat4 baseModel = scale(mat4(1.0f), vec3(scaleFactor, scaleFactor, scaleFactor));
	baseModel = rotate(baseModel, radians(rotationAngle), vec3(0.0f, 1.0f, 0.0f));

	// Torso
	mat4 torso = scale(baseModel, vec3(1.0f, 2.0f, 0.5f));
	DrawCube(torso, modelLoc);

	// Head
	mat4 head = translate(baseModel, vec3(0.0f, 1.5f, 0.0f));
	head = scale(head, vec3(0.7f, 0.7f, 0.7f));
	DrawCube(head, modelLoc);

	// Arms
	mat4 rightArm = translate(baseModel, vec3(1.0f, 0.5f, 0.0f));
	rightArm = scale(rightArm, vec3(0.5f, 1.5f, 0.5f));
	DrawCube(rightArm, modelLoc);

	mat4 leftArm = translate(baseModel, vec3(-1.0f, 0.5f, 0.0f));
	leftArm = scale(leftArm, vec3(0.5f, 1.5f, 0.5f));
	DrawCube(leftArm, modelLoc);

	// Legs
	mat4 rightLeg = translate(baseModel, vec3(0.3f, -1.5f, 0.0f));
	rightLeg = scale(rightLeg, vec3(0.5f, 1.5f, 0.5f));
	DrawCube(rightLeg, modelLoc);

	mat4 leftLeg = translate(baseModel, vec3(-0.3f, -1.5f, 0.0f));
	leftLeg = scale(leftLeg, vec3(0.5f, 1.5f, 0.5f));
	DrawCube(leftLeg, modelLoc);
}


void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);

	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

	// Projection and view matrices
	mat4 projection = perspective(radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
	mat4 view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -10.0f)); // Move camera farther

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

	glBindVertexArray(VAO);
	RenderBody(modelLoc);
	glBindVertexArray(0);
}




int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Rotating 3D Body", sf::Style::Close, context);

	glewInit();
	Init();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		rotationAngle += 0.5f; // Update rotation angle
		Render();
		window.display();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	return 0;
}