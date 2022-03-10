package main

import (
	"fmt"

	"github.com/go-gl/glfw/v3.3/glfw"
)

// store both the key and its representation to reduce code complexity
type pressedKey struct {
	key glfw.Key

	// representation, empty string if there isn't one
	repr string
}

// currently pressed keys
var pressedKeys = make([]pressedKey, 0)

// last pressed key
var lastKey glfw.Key

func initWin(width, height int, title string) *glfw.Window {
	err := glfw.Init()
	if err != nil {
		panic(err)
	}

	window, err := glfw.CreateWindow(width, height, title, nil, nil)
	if err != nil {
		panic(err)
	}

	window.SetKeyCallback(keyCallback)
	window.SetCharCallback(charCallback)
	window.SetCursorPosCallback(cursorPosCallback)

	return window
}

// maps keys to strings
func keyToString(k glfw.Key) (string, error) {
	switch k {
	case glfw.KeyLeftControl, glfw.KeyRightControl:
		return "_C", nil
	case glfw.KeyLeftAlt:
		return "_A", nil
	case glfw.KeyRightAlt: // right alt = alt gr
		return "_G", nil
	case glfw.KeyLeftShift, glfw.KeyRightShift:
		return "_S", nil
	case glfw.KeyLeftSuper, glfw.KeyRightSuper:
		return "_X", nil
	case glfw.KeyEscape:
		return "_E", nil
	case glfw.KeyEnter:
		return "_N", nil
	case glfw.KeyDelete:
		return "_D", nil
	case glfw.KeyInsert:
		return "_I", nil
	case glfw.KeyPrintScreen:
		return "_P", nil
	case glfw.KeyBackspace:
		return "_B", nil
	case glfw.KeyLeft:
		return "_<", nil
	case glfw.KeyRight:
		return "_>", nil
	case glfw.KeyUp:
		return "_^", nil
	case glfw.KeyDown:
		return "_V", nil

	case glfw.KeyF1:
		return "F1", nil
	case glfw.KeyF2:
		return "F2", nil
	case glfw.KeyF3:
		return "F3", nil
	case glfw.KeyF4:
		return "F4", nil
	case glfw.KeyF5:
		return "F5", nil
	case glfw.KeyF6:
		return "F6", nil
	case glfw.KeyF7:
		return "F7", nil
	case glfw.KeyF8:
		return "F8", nil
	case glfw.KeyF9:
		return "F9", nil
	case glfw.KeyF10:
		return "F10", nil
	case glfw.KeyF11:
		return "F11", nil
	case glfw.KeyF12:
		return "F12", nil
	}

	// unknown key
	return "", fmt.Errorf("unknown key")
}

// maps mouse buttons to strings, missing scroll wheel
func mouseToString(m glfw.MouseButton) string {
	switch m {
	case glfw.MouseButtonLeft:
		return "lc"
	case glfw.MouseButtonMiddle:
		return "mc"
	case glfw.MouseButtonRight:
		return "rc"
	}

	return ""
}

func cursorPosCallback(w *glfw.Window, xpos float64, ypos float64) {
	pkt := ClientInputSigPkt{source: 1, ispress: false, mposx: int(xpos), mposy: int(ypos), keys: []string{}}
	//fmt.Println("mouse moved! x =", xpos, "y =", ypos) //DEBUG

	sendClientInputSigPkt(udpConn, pkt)
}

// handle modifier keys and system command keys
func keyCallback(_ *glfw.Window, key glfw.Key, _ int, action glfw.Action, _ glfw.ModifierKey) {

	//fmt.Println("key pressed!", key, ", action:", action) //DEBUG

	// add to/remove from pressedKeys and send the modified combination of pressed keys
	if action == glfw.Press {
		toStr, err := keyToString(key)
		if err != nil {
			// let the char callback add that key to pressedKeys
			return
		}

		// new pressed key
		pk := pressedKey{key: key, repr: toStr}

		lastKey = key
		pressedKeys = append(pressedKeys, pk)
	} else {
		// find key and remove it
		for i := range pressedKeys {
			if pressedKeys[i].key == key {
				pressedKeys = append(pressedKeys[:i], pressedKeys[i:]...)
				break
			}
		}

		// packet
		pkt := ClientInputSigPkt{source: 2, ispress: false, mposx: -1, mposy: -1}

		// keys as strings
		pkt.keys = make([]string, len(pressedKeys))
		for i, k := range pressedKeys {
			pkt.keys[i] = k.repr
		}

		// send only when the key is released because charCallback already sends it
		// when the key is pressed
		sendClientInputSigPkt(udpConn, pkt)
	}
}

func charCallback(_ *glfw.Window, r rune) {

	// find the last added key and update the representation
	// the last added key is usually placed in the last position, but I'm not sure
	// if this is still true in such an asynchronous case
	for _, k := range pressedKeys {
		if k.key == lastKey {
			k.repr = string(r)
			break
		}
	}

	// packet
	pkt := ClientInputSigPkt{source: 2, mposx: -1, mposy: -1}

	//fmt.Println("char typed!", r) //DEBUG

	// send packet
	sendClientInputSigPkt(udpConn, pkt)
}
