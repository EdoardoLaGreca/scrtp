package main

import (
	"fmt"
	"net"
	"os"

	"github.com/go-gl/glfw/v3.3/glfw"
	"golang.org/x/crypto/sha3"
)

type WindowList []struct {
	name string
	id   int
}

const version string = "1.0"

func printUsage() {
	fmt.Println("Usage: scrtp <addr>:<port>")
}

func getHashedPassword(remAddr net.Addr) string {
	pw := askPassword(remAddr)
	pwBytes := sha3.Sum256([]byte(pw))

	return string(pwBytes[:])
}

func printWindowList(windows WindowList) {
	var idMaxLen int

	// find max ID length
	for _, w := range windows {
		idCurrLen := len(string(w.id))

		if idCurrLen > idMaxLen {
			idMaxLen = idCurrLen
		}
	}

	// print list
	for _, w := range windows {
		fmt.Print(w.id)

		for i := len(string(w.id)); i < idMaxLen+1; i++ {
			fmt.Print(" ")
		}

		fmt.Print(w.name)
	}
}

func main() {
	err := checkArgs()
	if err != nil {
		printUsage()
		panic(err)
	}

	// enables the usage of generated stuff
	RegisterGeneratedResolver()

	// TCP and UDP connection
	var conn net.Conn

	// set timeout
	var dialer net.Dialer
	dialer.Timeout = timeout

	// Establish TCP connection for authentication
	conn, err = dialer.Dial("tcp", os.Args[1])
	if err != nil {
		panic(err)
	}

	pw := getHashedPassword(conn.RemoteAddr())

	sendAuthPkt(conn, AuthPkt{hpw: pw, ver: version})

	serverReply := recvAuthRepPkt(conn)

	if !serverReply.ok {
		panic(serverReply.issue)
	}

	fmt.Println("Choose a window from the following list by typing its ID:")
	printWindowList(serverReply.windows)
	
	// check if input is valid
	for {
		fmt.Print("ID: ")

		var winID int
		fmt.Scanf("%d", &winID)

		for 
	}
	

	window := initWin(500, 500, "scrtp - " + )

	defer window.Destroy()

	window.SetKeyCallback(keyCallback)
	window.SetCharCallback(charCallback)

	for !window.ShouldClose() {
		// Do OpenGL stuff.
		window.SwapBuffers()
		glfw.PollEvents()
	}
}
