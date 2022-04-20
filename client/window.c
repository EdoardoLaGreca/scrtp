#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "print.h"
#include "net.h"

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    printf("key_callback: %d %d %d %d\n", key, scancode, action, mods); /*DEBUG*/
}

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
		print_err("failed to create an OpenGL context");
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
	/* loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* swap front and back buffers */
		glfwSwapBuffers(window);

		/* poll for and process events */
		glfwPollEvents();
	}
}

void
window_set_callbacks(GLFWwindow* window)
{
	glfwSetKeyCallback(window, key_callback);
}
