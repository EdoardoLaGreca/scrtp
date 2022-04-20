#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "window.h"
#include "args.h"
#include "net.h"

int
main(int argc, char** argv)
{
	if (!args_scan(argc, argv)) {
		exit(EXIT_FAILURE);
	}

	METADATA = net_get_metadata(HOSTNAME, PORT, 0);

	/* TODO: do network things */

	GLFWwindow* window = window_create(640, 480, "Hello World");

	window_set_callbacks(window);

	window_loop(window);

	return 0;
}