#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "print.h"
#include "net.h"

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	packet kbdev;
	unsigned char data[2] = { action, scancode };

	if (action != GLFW_PRESS && action != GLFW_RELEASE) {
		/* if key was not pressed or released, ignore it */
		return;
	}

	net_create_packet(0, "kbdev", data, sizeof(data));
	net_send_packet(&kbdev);
}

static void
mousebtn_callback(GLFWwindow* window, int button, int action, int mods)
{
	packet msclk;
	unsigned char data[2] = { action, button };

	if (action != GLFW_PRESS && action != GLFW_RELEASE) {
		/* if mouse was not pressed or released, ignore it */
		return;
	}

	net_create_packet(0, "msclk", data, sizeof(data));
	net_send_packet(&msclk);
}

static void
mousepos_callback(GLFWwindow* window, double xpos, double ypos)
{
	packet mspos;
	unsigned int data[4] = { (unsigned int) xpos, (unsigned int) ypos };

	net_create_packet(0, "msmv", data, sizeof(data));
	net_send_packet(&mspos);
}

GLFWwindow*
window_create(int width, int height, const char* title)
{
	GLFWwindow* window;

	if (!glfwInit()) {
		print_err("glfwInit() failed");
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
	glfwSetMouseButtonCallback(window, mousebtn_callback);
	glfwSetCursorPosCallback(window, mousepos_callback);
}
