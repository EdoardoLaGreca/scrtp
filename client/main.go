package main

import (
	"github.com/go-gl/glfw/v3.3/glfw"
)

func main() {

	window := initWin(500, 500, "hello world")

	defer window.Destroy()

	window.SetKeyCallback(keyCallback)
	window.SetCharCallback(charCallback)

	for !window.ShouldClose() {
		// Do OpenGL stuff.
		window.SwapBuffers()
		glfw.PollEvents()
	}
}
