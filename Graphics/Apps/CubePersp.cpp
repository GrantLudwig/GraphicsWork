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
float tranSpeed = .001f;

vec3 mouseDown(0, 0, 0);
vec3	rotOld(0, 0, 0),
		rotNew(0, 0, 0);
vec3	tranOld(0, 0, -1),
		tranNew(0, 0, -1);

float fieldOfView = 30;
float cubeSize = .05f;
float cubeStretch = cubeSize;

float	left = -1,
		right = 1,
		bottom = -1,
		top = 1,
		n = -1,
		f = 1;

float points[][3] = {
						{left, bottom, n},
						{left, bottom, f}, 
						{left, top, n},
						{left, top, f},
						{right, bottom, n},
						{right, bottom, f},
						{right, top, n},
						{right, top, f}
					};

float colors[][3] = {
						{0,0,1},
						{0,1,1},
						{0,0,0},
						{0,1,0},
						{1,0,1},
						{1,1,1},
						{1,0,0},
						{1,1,0}
					};

int faces[][4] = {
					{1,3,2,0},
					{6,7,5,4},
					{4,5,1,0},
					{3,7,6,2},
					{2,6,4,0},
					{5,7,3,1}
				};

const char *vertexShader = "\
	#version 130								\n\
	in vec3 point;								\n\
	in vec3 color;								\n\
	out vec4 vColor;							\n\
	uniform mat4 view;							\n\
	void main() {								\n\
		gl_Position = view * vec4(point, 1);	\n\
	    vColor = vec4(color, 1);				\n\
	}";

const char *pixelShader = "\
	#version 130								\n\
	in vec4 vColor;								\n\
	out vec4 pColor;							\n\
	void main() {								\n\
        pColor = vColor;						\n\
	}";

void Display(GLFWwindow *window) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// clear background
    glClearColor(.5,.5,.5,1);
    glClear(GL_COLOR_BUFFER_BIT);
	// access GPU vertex buffer
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    // associate position input to shader with position array in vertex buffer
	VertexAttribPointer(program, "point", 3, 0, (void *) 0);
    // associate color input to shader with color array in vertex buffer
	VertexAttribPointer(program, "color", 3, 0, (void *) sizeof(points));

	int screenWidth, screenHeight;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	int halfWidth = screenWidth / 2;
	float aspectRatio = (float) halfWidth / (float)screenHeight;

	float nearDistance = .001f, farDistance = 500;
	mat4 perp = Perspective(fieldOfView, aspectRatio, nearDistance, farDistance);
	mat4 scale = Scale(cubeSize, cubeSize, cubeStretch);
	mat4 rot = RotateY(rotNew.x) * RotateX(rotNew.y);
	mat4 tran = Translate(tranNew);
	mat4 modelView = tran * rot * scale;
	mat4 view = perp * modelView;
	//mat4 view = Translate(tranNew.x, tranNew.y, 0) * RotateY(rotNew.x) * RotateX(rotNew.y) * RotateZ(rotNew.z);
	SetUniform(program, "view", view);

	glViewport(0, 0, halfWidth, screenHeight);
	glDrawElements(GL_QUADS, sizeof(faces) / sizeof(int), GL_UNSIGNED_INT, faces);

	glViewport(halfWidth, 0, halfWidth, screenHeight);
	glLineWidth(5);
	for (int i = 0; i < 6; i++) {
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, &faces[i]);
	}
		 
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
	bool shift = mods & GLFW_MOD_SHIFT;
    if (action == GLFW_PRESS)
		switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case 'F':
				fieldOfView += shift ? -5 : 5;
				fieldOfView = fieldOfView < 5 ? 5 : fieldOfView > 150 ? 150 : fieldOfView;
				break;
			case 'S':
				cubeStretch *= shift ? .9f : 1.1f;
				cubeStretch = cubeStretch < .02f ? .02f : cubeStretch;
				break;
		}
		
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
			tranNew = tranOld + tranSpeed * vec3(dif.x, -dif.y, 1);
		else
			rotNew = rotOld + rotSpeed * dif;
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double direction) {
	/*rotNew = rotOld + zRotSpeed * vec3 (0, 0, (float)direction);
	rotOld = rotNew;*/

	tranNew.z += -(float)direction > 0 ? -.1f : .1f;
	tranOld.z = tranNew.z;
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
    GLFWwindow *w = glfwCreateWindow(1920, 1080, "Camera Cube", NULL, NULL);
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
		Display(w);
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
 	Close();
	glfwDestroyWindow(w);
    glfwTerminate();
}
