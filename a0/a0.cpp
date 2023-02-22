// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

namespace{
	std::vector<std::vector<unsigned short>> VERTEX_ATTRIBUTE_INDICES_BY_OBJECT;
	int DIFF_COLOR_INDEX = 0;
	int OBJECT_FILE_INDEX = 0;
}

// Controls in particular written along:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
namespace Controls {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::vec3 position = glm::vec3(0, 0, 5);
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float FoV = 45.0f;

	float speed = 3.0f;
	float mouseSpeed = 0.005f;

	GLfloat lightPositionUD = 1.0f;
	GLfloat lightPositionLR = 1.0f;

	void computeLight() {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			lightPositionUD += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			lightPositionUD -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			lightPositionLR += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			lightPositionLR -= 0.1f;
		}
	}

	void computeMatricesFromInputs() {

		// only needed once, hence static
		static double lastTime = glfwGetTime();

		// time since last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, 1024 / 2, 768 / 2);

		horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
		verticalAngle += mouseSpeed * float(768 / 2 - ypos);
		// prevents looking backwards by looking upside-down.
		// alternatively: locks vertical angle between straight up and straight down.
		verticalAngle = std::min(1.57f, std::max(-1.57f, verticalAngle));

		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);
		glm::vec3 up = glm::cross(right, direction);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position += direction * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position -= direction * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position += right * deltaTime * speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position -= right * deltaTime * speed;
		}

		projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
		viewMatrix = glm::lookAt(
			position,           // Camera is here
			position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);


		lastTime = currentTime;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		DIFF_COLOR_INDEX = (DIFF_COLOR_INDEX + 1) % 4;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		OBJECT_FILE_INDEX = (OBJECT_FILE_INDEX + 1) % 5; // 4 objects

	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, VERTEX_ATTRIBUTE_INDICES_BY_OBJECT[OBJECT_FILE_INDEX].size() * sizeof(unsigned short), &VERTEX_ATTRIBUTE_INDICES_BY_OBJECT[OBJECT_FILE_INDEX][0], GL_STATIC_DRAW);

}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "assignment0", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	//// Dark blue background
	//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

//	glfwSetKeyCallback(window, key_callback);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");

	// -- MIT OCW a0 code (kind of modified, replaced deprecated functions) -- //
	// Here are some colors you might use - feel free to add more
	std::vector<glm::vec4> diffColors{ {0.5, 0.5, 0.9, 1.0},
								 {0.9, 0.5, 0.5, 1.0},
								 {0.5, 0.9, 0.3, 1.0},
								 {0.3, 0.8, 0.9, 1.0} };
	GLuint diffColorID = glGetUniformLocation(programID, "diffColor");

	// Light position
	GLuint lightPositionID = glGetUniformLocation(programID, "lightPosition_modelspace");

	// Get a handle for our "MVP" uniform
	GLuint mID = glGetUniformLocation(programID, "M");
	GLuint vID = glGetUniformLocation(programID, "V");
	GLuint pID = glGetUniformLocation(programID, "P");

	char* objectFiles[5] = { "suzanne.obj", "torus.obj", "sphere.obj",  "cube.obj", "teapot.obj" };

	// Read our .obj file
	std::vector<glm::vec3> indexed_vertices, indexed_normals;
	int objectFileCount = 0;
	for (char* str : objectFiles) {
		std::vector<glm::vec3> vertices, normals;
		std::vector<glm::vec2> uv;
		std::vector<unsigned short> indices;
		const char* objectFile = objectFiles[objectFileCount];
		loadOBJ(objectFile, vertices, uv, normals);
		indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);
		VERTEX_ATTRIBUTE_INDICES_BY_OBJECT.push_back(indices);
		objectFileCount++;
	}
	// Load it into a VBO
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, VERTEX_ATTRIBUTE_INDICES_BY_OBJECT[OBJECT_FILE_INDEX].size() * sizeof(unsigned short), &VERTEX_ATTRIBUTE_INDICES_BY_OBJECT[OBJECT_FILE_INDEX][0], GL_STATIC_DRAW);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0) {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		Controls::computeLight();
		int diffColorIndex = DIFF_COLOR_INDEX;
		int objectFileIndex = OBJECT_FILE_INDEX;
		GLfloat lightPositionUD = Controls::lightPositionUD;
		GLfloat lightPositionLR = Controls::lightPositionLR;


		glUniform3f(lightPositionID, lightPositionLR, lightPositionUD, 5.0f); // set light position at z=5
		glUniform3f(diffColorID, diffColors[diffColorIndex][0], diffColors[diffColorIndex][1], diffColors[diffColorIndex][2]);
		Controls::computeMatricesFromInputs();
		mat4 M = glm::mat4(1.0f);
		mat4 V = Controls::viewMatrix;
		mat4 P = Controls::projectionMatrix;
		glUniformMatrix4fv(mID, 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(vID, 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(pID, 1, GL_FALSE, &P[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		glDrawElements(
			GL_TRIANGLES,
			VERTEX_ATTRIBUTE_INDICES_BY_OBJECT[0].size(),
			GL_UNSIGNED_SHORT,
			(void*)0
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//End of each loop:

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

