package client

import (
	"runtime"
	
	"github.com/go-gl/glfw/v3.3/glfw"
)

func createWindow(width, height int) (glfw.Window, error) {
	err := glfw.Init()

	if err != nil {
		return nil, err
	}

	defer glfw.Terminate()

	window, err := glfw.CreateWindow(width, height, "scrtp", nil, nil)

	if err != nil {
		return nil, err
	}

	window.MakeContextCurrent()

	return window
}

func updateWindow(window *glfw.Window, part WindowFrame) error {
	
}
