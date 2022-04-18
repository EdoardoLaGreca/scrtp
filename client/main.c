#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "window.h"

int
main(void)
{
	/* TODO: do network things */

	GLFWwindow *window = window_create(640, 480, "Hello World");

	window_set_callbacks(window);

	window_loop(window);

	return 0;
}