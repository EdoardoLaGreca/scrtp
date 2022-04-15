#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

GLFWwindow*
create_window(int width, int height, const char* title)
{
	GLFWwindow* window;

	if (!glfwInit()) {
		
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (!window) {
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