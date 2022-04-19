#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "print.h"
#include "net.h"

GLFWwindow*
window_create(int width, int height, const char* title)
{
	GLFWwindow* window;

	if (!glfwInit()) {
		perr("glfwInit() failed");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (!window) {
		perr("glfwCreateWindow() failed");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	return window;
}

void
window_loop(GLFWwindow* window)
{
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    printf("key_callback: %d %d %d %d\n", key, scancode, action, mods); /*DEBUG*/
}

void
window_set_callbacks(GLFWwindow* window)
{
	glfwSetKeyCallback(window, key_callback);
}
