package main

import (
	x "github.com/linuxdeepin/go-x11-client"
	"github.com/linuxdeepin/go-x11-client/util/wm/ewmh"
)

type window struct {
	name string
	id   x.Window
	geom geometry
}

type geometry struct {
	x      int16
	y      int16
	width  uint16
	height uint16
}

type pixel [3]byte

// X11 connection
var xConn *x.Conn

func init() {
	conn, err := x.NewConn()
	if err != nil {
		panic(err)
	}

	xConn = conn
}

func getWindowList() []window {
	reply, err := ewmh.GetClientList(xConn).Reply(xConn)
	if err != nil {
		printDebug("unable to get currently open windows: " + err.Error())
		return make([]window, 0)
	}

	windowList := make([]window, len(reply))

	for i, w := range reply {
		// if Reply returns error, just keep the empty string
		windowName, _ := ewmh.GetWMName(xConn, w).Reply(xConn)
		windowGeom, _ := getWinGeom(w)
		windowList[i] = window{name: windowName, id: w, geom: windowGeom}
	}

	return windowList
}

// convert windows to anonymous struct
func convertWinsToAnonStruct(windows []window) []struct {
	name string
	id   int
} {
	converted := make([]struct {
		name string
		id   int
	}, 0)

	for _, w := range windows {
		converted = append(converted, struct {
			name string
			id   int
		}{
			name: w.name,
			id:   int(w.id),
		})
	}

	return converted
}

// is window ID valid?
func isWinIDValid(id int) bool {

	// find window by window ID
	for _, w := range getWindowList() {
		if int(w.id) == id {
			return true
		}
	}

	return false
}

// id must be a valid window ID
// activate window by switching to its desktop and raising it
func activateWin(id x.Window) {
	ewmh.SetActiveWindow(xConn, id)
}

// get window geometry (a.k.a. the x and y coordinates of the top-left corner
// with width and height)
func getWinGeom(id x.Window) (geometry, error) {
	geom, err := x.GetGeometry(xConn, x.Drawable(id)).Reply(xConn)
	if err != nil {
		return geometry{}, err
	}

	return geometry{
		x:      geom.X,
		y:      geom.Y,
		width:  geom.Width,
		height: geom.Height,
	}, nil
}

func getWindowFrame(w *window) ([]byte, error) {
	reply, err := x.GetImage(xConn, x.ImageFormatZPixmap, x.Drawable(w.id), w.geom.x, w.geom.y, w.geom.width, w.geom.height, 0xFFFFFFFF).Reply(xConn)
	if err != nil {
		return nil, err
	}

	return reply.Data, nil
}
