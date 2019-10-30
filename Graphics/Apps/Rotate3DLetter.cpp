// SimplestTriangle.cpp: draw colorful triangle via GLSL, vBuffer

#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include "GLXtras.h"

// GPU identifiers
GLuint vBuffer = 0;
GLuint program = 0;

float rotSpeed = .3f;
float zRotSpeed = 2;
float tranSpeed = .01f;

vec3 mouseDown(0, 0, 0);
vec3	rotOld(0, 0, 0),
		rotNew(0, 0, 0);
vec2	tranOld(0, 0),
		tranNew(0, 0);

// vertices
float  points[][2] = {		{.833f, 1},
							{.833f, .667f},
							{.333f, .667f},
							{0, 1},
							{-.333f, .333f},
							{-.667f, .667f},
							{-1, .167f},
							{-.667f, 0},
							{-.667f, -.667f},
							{-.333f, -.333f},
							{0, -1},
							{0, -.667f},
							{.667f, -.833f},
							{1, -.167f},
							{1, .167f},
							{0, .167f},
							{0, -.167f},
							{.667f, -.167f}
					};

float  colors[][3] =	{	{1, 0, 0},
							{.875f, .125f, 0},
							{.75f, .25f, 0},
							{.625f, .375f, 0},
							{.5f, .5f, 0},
							{.375f, .625f, 0},
							{.25f, .75f, 0},
							{.125f, .875f, 0},
							{0, 1, 0},
							{0, .889f, .111f},
							{0, .778f, .222f},
							{0, .667f, .333f},
							{0, .556f, .444f},
							{0, .445f, .555f},
							{0, .334f, .666f},
							{0, 0, 1},
							{0, .112f, .888f},
							{0, .223f, .777f}
						};

// triangles
int triangles[][3] =	{	{0, 1, 2},
							{0, 2, 3},
							{2, 3, 4},
							{3, 5, 4},
							{4, 5, 7},
							{5, 7, 6},
							{6, 7, 8},
							{7, 8, 9},
							{8, 9, 11},
							{8, 10, 11},
							{10, 11, 12},
							{11, 12, 17},
							{12, 17, 13},
							{17, 13, 14},
							{15, 14, 17},
							{15, 17, 16}
						};

const char *vertexShader = "\
	#version 130								\n\
	in vec2 point;								\n\
	in vec3 color;								\n\
	out vec4 vColor;							\n\
	uniform mat4 view;							\n\
	void main() {								\n\
		gl_Position = view * vec4(point, 0, 1);	\n\
	    vColor = vec4(color, 1);				\n\
	}";

const char *pixelShader = "\
	#version 130								\n\
	in vec4 vColor;								\n\
	out vec4 pColor;							\n\
	void main() {								\n\
        pColor = vColor;						\n\
	}";

void Display() {
	mat4 view = Translate(tranNew.x, tranNew.y, 0) * RotateY(rotNew.x) * RotateX(rotNew.y) * RotateZ(rotNew.z);
	SetUniform(program, "view", view);
	// clear background
    glClearColor(.5,.5,.5,1);
    glClear(GL_COLOR_BUFFER_BIT);
	// access GPU vertex buffer
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    // associate position input to shader with position array in vertex buffer
	VertexAttribPointer(program, "point", 2, 0, (void *) 0);
    // associate color input to shader with color array in vertex buffer
	VertexAttribPointer(program, "color", 3, 0, (void *) sizeof(points));
	// render three vertices as a triangle
	int nvertices = sizeof(triangles) / sizeof(int);
	glDrawElements(GL_TRIANGLES, nvertices, GL_UNSIGNED_INT, triangles);
    glFlush();
}

void InitVertexBuffer() {
    // make GPU buffer for points & colors, set it active buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    // allocate buffer memory to hold vertex locations and colors
	int sPnts = sizeof(points), sCols = sizeof(colors);
    glBufferData(GL_ARRAY_BUFFER, sPnts+sCols, NULL, GL_STATIC_DRAW);
    // load data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, sPnts, points);
        // start at beginning of buffer, for length of points array
    glBufferSubData(GL_ARRAY_BUFFER, sPnts, sCols, colors);
        // start at end of points array, for length of colors array
}

bool InitShader() {
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	if (!program)
		printf("can't init shader program\n");
	return program != 0;
}

// application

void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	if (action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(w, &x, &y);
		mouseDown = vec3((float)x, (float)y, 0);
	}
	if (action == GLFW_RELEASE) {
		rotOld = rotNew;
		tranOld = tranNew;
	}
}

void MouseMove(GLFWwindow *w, double x, double y) {
	bool shift = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		/*vec2 dif((float)x - mouseDown.x, (float)y - mouseDown.y);
		rotNew = rotOld + rotSpeed * dif;*/
		vec3 mouse((float)x, (float)y, 0), dif = mouse - mouseDown;

		if (shift)
			tranNew = tranOld + tranSpeed * vec2(dif.x, -dif.y);
		else
			rotNew = rotOld + rotSpeed * dif;
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double direction) {
	rotNew = rotOld + zRotSpeed * vec3 (0, 0, (float)direction);
	rotOld = rotNew;
}

void ErrorGFLW(int id, const char *reason) {
    printf("GFLW error %i: %s\n", id, reason);
}

void Close() {
	// unbind vertex buffer and free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBuffer);
}

int main() {
    glfwSetErrorCallback(ErrorGFLW);
    if (!glfwInit())
        return 1;
    GLFWwindow *w = glfwCreateWindow(500, 500, "Rotate 3D Letter", NULL, NULL);
    if (!w) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));
    PrintGLErrors();
	if (!InitShader())
        return 0;
    InitVertexBuffer();
    glfwSetKeyCallback(w, Keyboard);
	glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetCursorPosCallback(w, MouseMove);
	glfwSetScrollCallback(w, MouseWheel);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
 	Close();
	glfwDestroyWindow(w);
    glfwTerminate();
}
