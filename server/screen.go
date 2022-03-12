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

// TODO: migrate most of these functions from using wmctrl to the X11 library

// current open windows
var openWindows = make([]window, 0)

func updateWindowsList() error {
	output, err := exec.Command("wmctrl", "-lG").Output()
	if err != nil {
		printDebug("could not get window list")
	}

	winListStr := strings.Split(string(output), "\n")

	openWindows = make([]window, 0)

	openWindows = append(openWindows, window{name: "Entire desktop", id: 0})

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

// is window ID valid?
func isWinIDValid(id int) bool {
	updateWindowsList()

	// find window by window ID
	for _, w := range openWindows {
		if w.id == id {
			return true
		}
	}

	return false
}

// id must be a valid window ID
// activate window by switching to its desktop and raising it
func activateWin(id int) {
	hexID := fmt.Sprintf("0x%x", id)

	err := exec.Command("wmctrl", "-i", "-a", hexID).Run()
	if err != nil {
		printDebug("could not activate window " + hexID)
	}
}

// get window geometry (a.k.a. the x and y coordinates of the top-left corner
// with width and height)
func getWinGeom(id int) (x int, y int, width int, height int) {
	hexID := fmt.Sprintf("0x%x", id)

	output, err := exec.Command("wmctrl", "-lG").Output()
	if err != nil {
		printDebug("could not get window list")
	}

}
