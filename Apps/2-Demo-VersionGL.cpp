// TestGL.cpp: an application to determine GL and GLSL versions

#define USE_GLFW

#ifdef USE_GL

#include "gl.h"
#include <stdio.h>

int main() {
	printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL version: %s\n", glGetString(GL_VERSION));
	getchar();
}

#endif
#ifdef USE_GLFW

#include "glad.h"
#include <glfw3.h>
#include <stdio.h>

int main() {
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(1, 1, "", NULL, NULL);
	glfwSetWindowPos(window, 0, 0);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	getchar();
}

#endif
#ifdef USE_GLUT

#include "glad.h"
#include "freeglut.h"
#include <stdio.h>
int main(int ac, char **av) {
	// initialize OpenGL extension entry points
	if (gladLoadGL()) {
        printf("failed to load OpenGL\n");
        return 1;
	}
	// initialize tiny application window
    glutInitWindowSize(1, 1);
    glutInitWindowPosition(0, 0);
    glutInit(&ac, av);
    glutCreateWindow(av[0]);
	// print GL info to console
	printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	// keep app open
    getchar();
    return 0;
}

#endif
