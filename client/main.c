#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "window.h"
#include "args.h"

int
main(int argc, char** argv)
{
	if (!args_scan(argc, argv)) {
		exit(EXIT_FAILURE);
	}

	/* TODO: do network things */

	GLFWwindow *window = window_create(640, 480, "Hello World");

	window_set_callbacks(window);

	window_loop(window);

	return 0;
}