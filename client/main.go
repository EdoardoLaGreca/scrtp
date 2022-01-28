package client

import (
	"crypto/aes"
	"crypto/cipher"
	"encoding/binary"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"

	"github.com/go-gl/glfw/v3.3/glfw"
)

type KeyboardInput struct {
	isPress bool // true when press, false when release
	isCombo bool // true when combination, false when character

	// these fields are accessed if isCombo is true
	mod int
	key int

	// these fields are accessed if isCombo is false
	char rune
}

// key size is 128 bit
const keySize int = 128 / 8

// channels to get input
var keyboardChan chan KeyboardInput = make(chan KeyboardInput)

func charCallback(w *glfw.Window, char rune) {
	input := KeyboardInput{
		isPress: true,
		isCombo: false,
		char:    char,
	}

	// simulate press and release
	keyboardChan <- input
	input.isPress = false
	keyboardChan <- input
}

func keyCallback(w *glfw.Window, key glfw.Key, scancode int, action glfw.Action, mods glfw.ModifierKey) {
	input := KeyboardInput{
		isCombo: true,
		mod:     modToEnum(mods),
		key:     keyToEnum(key),
	}

	if action == glfw.Press {
		input.isPress = true
	} else if action == glfw.Release {
		input.isPress = false
	} else { // action is glfw.Repeat
		// simulate press and release
		input.isPress = true
		keyboardChan <- input
		input.isPress = false
		keyboardChan <- input
	}

	keyboardChan <- input
}

// map key to enum
func keyToEnum(key glfw.Key) int {
	// find a way

	return 0
}

// map modifier to enum
func modToEnum(mod glfw.ModifierKey) int {
	switch mod {
	case glfw.ModShift:
		return 1
	case glfw.ModControl:
		return 2
	case glfw.ModAlt:
		return 3
	case glfw.ModSuper:
		return 4
	}

	// unrecognized
	return 255
}

// TODO: better organization of code
func main() {
	if len(os.Args) != 3 {
		// usage: scrtp <ip> <port>
		fmt.Println("Usage: scrtp <ip> <port>")
		os.Exit(1)
	}

	ip, port := os.Args[1], os.Args[2]
	address := fmt.Sprintf("%s:%s", ip, port)

	log.Println("Scrtp client started")
	log.Println("Getting the current user's AES key...")

	key, err := readKey()

	if err != nil {
		log.Fatalln("An error occurred while reading the key:", err)
	}

	log.Println("Creating a block cipher using the AES key...")

	ciph, err := aes.NewCipher(key)

	if err != nil {
		log.Fatalln("An error occurred while creating the block cipher:", err)
	}

	log.Println("Establishing a TCP connection...")

	conn, err := net.DialTimeout("tcp", address, time.Second*5)

	if err != nil {
		log.Fatalln("An error occurred while trying to connect to", "\""+address+"\":", err)
	}

	defer conn.Close()

	log.Println("Getting the current machine's MAC address...")

	MACaddr, err := getMACaddr("tcp", address)

	if err != nil {
		log.Fatalln("An error occurred while trying to get the MAC address:", err)
	}

	// step 1: send auth packet
	log.Println("Sending the authentication packet...")
	sendEnc(conn, ciph, buildAuthPkt(key, MACaddr, "1.0", "0"))

	// step 2: read the reply
	reply := make([]byte, 2)
	var restOfPkt string

	if _, err := conn.Read(reply); err != nil {
		log.Fatalln("An error occurred while trying to read the server's reply:", err)
	}

	if string(reply) != "OK" && string(reply) != "NO" {
		log.Fatalln("The remote server sent a malformed reply:", reply)
	}

	// read the rest of the packet
	bytes, err := receiveAllDec(conn, ciph)

	if err != nil {
		log.Fatalln("An error occurred while trying to read the server's reply:", err)
	}

	restOfPkt = string(bytes)

	if string(reply) == "NO" {
		// restOfPkt contains the issue
		log.Fatalln("The remote server refused the request:", restOfPkt)
	}

	// restOfPkt contains the list of windows
	windows := strings.Split(restOfPkt, "\n")

	// add entry "Entire desktop" as first element
	windows = append(append(windows[:0], "Entire desktop"), windows...)

	fmt.Println("Available windows:")
	for n, w := range windows {
		fmt.Printf("%d - %s\n", n, w)
	}

	var chosenWin int

	fmt.Print("Select one by its number: ")
	fmt.Scanf("%d", &chosenWin)

	// step 3: send the chosen window
	if chosenWin == 0 {
		sendEnc(conn, ciph, []byte("0"))
	} else {
		sendEnc(conn, ciph, []byte(windows[chosenWin]))
	}

	window, err := createWindow(500, 500)

	window.SetCharCallback(charCallback)

	if err != nil {
		log.Fatalln("Cannot create a window:", err)
	}

	// step 4: get frames (concurrently)
	go func(conn net.Conn, ciph cipher.Block, window *glfw.Window) {
		for {
			wf, err := receiveWinFrame(conn, ciph)

			if err != nil {
				log.Println("An error occurred while trying to receive a window frame:", err)
			}

			updateWindow(window, *wf)
		}
	}(conn, ciph, window)

	// step 5: send input signals (concurrently)
	go func() {
		stopSending := false

		for !stopSending {
			kinput := <-keyboardChan

			var t int
			val1 := make([]byte, 4)
			val2 := make([]byte, 4)

			if kinput.isPress {
				t = 1
			} else {
				t = 0
			}

			if kinput.isCombo {
				val1[0] = byte(kinput.mod)
				val1[1] = byte(kinput.key)
			} else {
				binary.BigEndian.PutUint32(val1, uint32(kinput.char))
			}

			sendInputSignal(conn, ciph, int8(t), val1, val2)
		}
	}()

	for !window.ShouldClose() {

		window.SwapBuffers()
		glfw.PollEvents()
	}
}
