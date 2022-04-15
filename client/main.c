#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "window.h"

int
main(void)
{
	GLFWwindow *window = create_window(640, 480, "Hello World");

	window_loop();

	return 0;
}