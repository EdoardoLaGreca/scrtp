package main

import (
	"fmt"
	"net"

	"github.com/go-gl/glfw/v3.3/glfw"
	"golang.org/x/crypto/sha3"
)

type Window struct {
	name string
	id   int
}

type WindowList []struct {
	name string
	id   int
}

// Protocol version
const protoVersion string = "1.0"

func getHashedPassword(remAddr net.Addr) string {
	pw := askPassword(remAddr)
	pwBytes := sha3.Sum256([]byte(pw))

	return string(pwBytes[:])
}

func printWindowList(windows WindowList) {
	var idMaxLen int

	// find max ID length
	for _, w := range windows {
		idCurrLen := len(fmt.Sprint(w.id))

		if idCurrLen > idMaxLen {
			idMaxLen = idCurrLen
		}
	}

	// print list
	for _, w := range windows {
		fmt.Print(w.id)

		for i := len(fmt.Sprint(w.id)); i < idMaxLen+1; i++ {
			fmt.Print(" ")
		}

		fmt.Print(w.name)
	}
}

func main() {
	// call function checkArgs as first thing, so that it fills the global variable CLIArgs
	err := checkArgs()
	if err != nil {
		printUsage()
		panic(err)
	}

	printDebug("arguments parsed")

	// enables the usage of generated stuff
	RegisterGeneratedResolver()

	// TCP and UDP connection
	var conn net.Conn

	// set timeout
	var dialer net.Dialer
	dialer.Timeout = timeout

	printDebug("dialing remote address...")

	// Establish TCP connection for authentication
	conn, err = dialer.Dial("tcp", CLIArgs.address)
	if err != nil {
		panic(err)
	}

	printDebug("connection established")

	pw := getHashedPassword(conn.RemoteAddr())

	printDebug("sending authentication packet...")

	sendAuthPkt(conn, AuthPkt{hpw: pw, ver: protoVersion})

	printDebug("authentication packet sent, waiting for a reply...")

	serverReply := recvAuthRepPkt(conn)

	printDebug("authentication reply received")

	if !serverReply.ok {
		panic(serverReply.issue)
	}

	fmt.Println("Choose a window from the following list by typing its ID:")
	printWindowList(serverReply.windows)

	var selectedWindow Window

	// repeat until input is valid
outer:
	for {
		fmt.Print("ID: ")

		var inputWinID int

		n, err := fmt.Scanf("%d", &inputWinID)
		if err != nil {
			panic(err)
		}

		if n == 0 {
			// no input integer value has been read
			fmt.Println("No integer value was detected, try again.")
			continue
		}

		// search ID
		for _, w := range serverReply.windows {
			if w.id == inputWinID {
				selectedWindow = Window{name: w.name, id: w.id}

				// the input ID is valid
				break outer
			}
		}

		fmt.Println("This ID does not exist, try again.")
	}

	printDebug("sending window ID packet...")

	sendWIDPkt(conn, WIDPkt{id: selectedWindow.id})

	printDebug("window ID sent, waiting for the first window frame...")

	wf := recvServerWinFramePkt(conn)

	printDebug("first window frame received, creating window...")

	window := initWin(wf.width, wf.height, "scrtp - "+selectedWindow.name)
	//window := initWin(500, 500, "scrtp") //DEBUG
	defer window.Destroy()

	// window loop
	for !window.ShouldClose() {
		window.SwapBuffers()
		glfw.PollEvents()
	}
}
