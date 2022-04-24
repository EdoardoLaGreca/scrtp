#pragma once

#include <GLFW/glfw3.h>

/* window size */
extern unsigned int WINDOW_WIDTH;
extern unsigned int WINDOW_HEIGHT;

/*
 * Create a new window with the given title and size.
 */
GLFWwindow* window_create(int width, int height, const char* title);

/*
 * Enter the main loop of the window.
 */
void window_loop(GLFWwindow* window);

/*
 * Set window callbacks.
 */
void window_set_callbacks(GLFWwindow* window);