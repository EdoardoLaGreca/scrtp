package main

import (
	"bytes"
	"errors"
	"image"
	"net"

	"github.com/BurntSushi/xgb/xproto"
	"github.com/BurntSushi/xgbutil"
	"github.com/BurntSushi/xgbutil/ewmh"
	"github.com/BurntSushi/xgbutil/xgraphics"
	"github.com/gen2brain/x264-go"
)

type window struct {
	name string
	id   xproto.Window
	geom geometry
}

type geometry struct {
	x      int16
	y      int16
	width  uint16
	height uint16
}

// XUtil connection
var xutil *xgbutil.XUtil

// desktop as a window
var desktop window = window{
	name: "Full desktop",
	id:   0,
}

func init() {
	// init X11 connection
	conn, err := xgbutil.NewConn()
	if err != nil {
		panic(err)
	}

	xutil = conn

	desktopGeom, err := getDesktopSize()
	if err != nil {
		panic(err)
	}

	// init desktop variable
	desktop.geom = desktopGeom
}

func frameSender(conn net.Conn, win window, preset string, stop chan<- struct{}) {
	// H.264 video output
	var videoOutput bytes.Buffer
	videoOutput.Grow(1024)

	// init encoder
	enc, err := x264.NewEncoder(&videoOutput, &x264.Options{
		Width:     int(win.geom.width),
		Height:    int(win.geom.height),
		FrameRate: 60,
		Tune:      "zerolatency",
		Preset:    preset,
		Profile:   "main",
		LogLevel:  0,
	})
	if err != nil {
		printDebug("error occurred during the H.264 encoder creation: " + err.Error())
		stop <- struct{}{}
	}

	var winGeom geometry

	for {

		// update window size
		if win.id == 0 {
			winGeom, err = getDesktopSize()
		} else {
			winGeom, err = getWinGeom(win.id)
		}

		if err != nil {
			printDebug("error occurred while getting window/desktop size: " + err.Error())
		} else {
			win.geom = winGeom
		}

		img, err := getWindowFrame(&win)
		if err != nil {
			printDebug("error occurred while getting a window frame: " + err.Error())
			conn.Close()
			stop <- struct{}{}
			return
		}

		err = enc.Encode(img)
		if err != nil {
			printDebug("error occurred while encoding the image: " + err.Error())
		}

		sendServerWinFramePkt(conn, ServerWinFramePkt{width: int(win.geom.width), height: int(win.geom.height), compfr: videoOutput.Bytes()})

	}
}

func inputReceiver(conn net.Conn, win window, stop chan<- struct{}) {
	for {

		// TODO
	}
}

func getWindowList() []window {
	reply, err := ewmh.ClientListGet(xutil)
	if err != nil {
		printDebug("unable to get currently open windows: " + err.Error())
		return make([]window, 0)
	}

	windowList := make([]window, len(reply))

	for i, w := range reply {
		// if Reply returns error, just keep the empty string
		windowName, _ := ewmh.WmNameGet(xutil, w)
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

// find window by its ID, also useful to check if the ID is correct
func findWindowByID(id int) (window, error) {
	if id == 0 {
		return desktop, nil
	}

	for _, w := range getWindowList() {
		if int(w.id) == id {
			return w, nil
		}
	}

	return window{}, errors.New("window not found")
}

/* may be useful in future
// id must be a valid window ID
// activate window by switching to its desktop and raising it
func activateWin(id xproto.Window) {
	ewmh.ActiveWindowSet(xutil, id)
}
*/

// get window geometry (a.k.a. the x and y coordinates of the top-left corner
// with width and height)
func getWinGeom(id xproto.Window) (geometry, error) {
	geom, err := xproto.GetGeometry(xutil.Conn(), xproto.Drawable(id)).Reply()
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

func getDesktopSize() (geometry, error) {
	desktopGeometry, err := ewmh.DesktopGeometryGet(xutil)
	if err != nil {
		printDebug("cannot get desktop geometry")
		return geometry{}, err
	}

	return geometry{x: 0, y: 0, width: uint16(desktopGeometry.Width), height: uint16(desktopGeometry.Height)}, nil

}

func getWindowFrame(w *window) (image.Image, error) {
	ximg, err := xgraphics.NewDrawable(xutil, xproto.Drawable(xutil.RootWin()))
	if err != nil {
		return nil, err
	}

	img := new(image.RGBA)
	img.Stride = ximg.Stride
	img.Rect = ximg.Rect
	img.Pix = ximg.Pix

	// convert ximg.Pix (BGRA) into RGBA by swapping B and R
	for i := 0; i < len(ximg.Pix); i += 4 {
		img.Pix[i], img.Pix[i+2] = img.Pix[i+2], img.Pix[i]
	}

	return img, nil
}

/* may be useful in future
// choose the video bitrate based on the window size
func chooseBitrate(winWidth, winHeight uint) uint {
	fps := uint(60)

	// this constant is used to map winHeight*winWidth*fps to bitrates
	k := 18.3

	return uint(float64(winWidth*winHeight*fps) / k)
}
*/
