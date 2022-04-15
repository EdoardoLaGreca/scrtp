#pragma once

#include <GLFW/glfw3.h>

/*
 * Create a new window with the given title and size.
 */
GLFWwindow* create_window(int width, int height, const char* title);

/*
 * Enter the main loop of the window.
 */
void window_loop();