package main

import (
	"fmt"
	"os/exec"
	"strings"
)

type window struct {
	name string
	id   int
}

// current open windows
var openWindows = make([]window, 0)

func updateWindowsList() error {
	output, err := exec.Command("wmctrl", "-l").Output()
	if err != nil {
		printDebug("could not get window list")
	}

	winListStr := strings.Split(string(output), "\n")

	openWindows = make([]window, 0)

	for _, s := range winListStr {
		var winID int
		var winName string

		fmt.Sscanf(s, "0x%x  %s", &winID, &winName)

		openWindows = append(openWindows, window{name: winName, id: winID})
	}

	return nil
}

// convert open windows to anonymous struct
func convertWinToAnonStruct() []struct {
	name string
	id   int
} {
	windows := make([]struct {
		name string
		id   int
	}, 0)

	for _, w := range windows {
		windows = append(windows, struct {
			name string
			id   int
		}{
			name: w.name,
			id:   w.id,
		})
	}

	return windows
}
