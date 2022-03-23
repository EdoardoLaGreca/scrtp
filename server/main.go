package main

import (
	"log"
	"math/rand"
	"net"
	"os"
)

type property struct {
	key   string
	value string
}

// properties
var props struct {
	listenOn       string
	isHelp         bool
	isDebug        bool
	hashedPassword string
}

func init() {
	fillProps()
}

// check if the program has enough privilege to access the /etc directory
func checkEtc() bool {
	dir, err := os.Open("/etc")
	defer func() {
		_ = dir.Close()
	}()

	return err != nil
}

// generate token
func genToken() string {
	// choose length (from 50 to 100)
	length := rand.Intn(50) + 50

	var token string

	for i := 0; i < length; i++ {
		token += string(rand.Intn(10))
	}

	return token

}

func handleTCPConnection(conn net.Conn) {
	printDebug("!! new connection from " + conn.RemoteAddr().String() + ", waiting for an authentication packet...")
	defer printDebug("connection of " + conn.RemoteAddr().String() + " closed")
	defer conn.Close()

	authPkt := recvAuthPkt(conn)

	printDebug("authentication packet received")

	var reply AuthRepPkt

	if authPkt.hpw != props.hashedPassword {
		printDebug("login from " + conn.RemoteAddr().String() + " failed becuse of wrong password")

		reply = AuthRepPkt{
			ok:    false,
			issue: "wrong password",
		}

		sendAuthRepPkt(conn, reply)

		return
	}

	printDebug("login from " + conn.RemoteAddr().String() + " successful")

	windowList := getWindowList()

	// add full desktop as first option
	windowList = append([]window{{name: "Desktop", id: 0}}, windowList...)

	reply = AuthRepPkt{
		ok:      true,
		issue:   "",
		windows: convertWinsToAnonStruct(windowList),
	}

	sendAuthRepPkt(conn, reply)

	printDebug("auth reply packet sent, waiting for window ID...")

	widPkt := recvWIDPkt(conn)
	chosenWindow, err := findWindowByID(widPkt.id)

	if err != nil {
		printDebug("window ID is invalid")
		return
	}

	stop := make(chan struct{})

	go frameSender(conn, chosenWindow, widPkt.quality, stop)

	go inputReceiver(conn, chosenWindow, stop)

	// finish on stop message
	<-stop
}

// fill props global variable
func fillProps() {
	err := checkArgs()
	if err != nil {
		printUsage()
		panic(err)
	}

	if !checkEtc() {
		panic("cannot access /etc")
	}

	config, err := readConfig()
	if err != nil {
		log.Println("An error occurred while reading config file:", err)
	}

	// fill global variable props with values from config file
	for _, prop := range config {
		switch prop.key {
		case "listen_on":
			props.listenOn = prop.value
		}
	}

	props.hashedPassword = getHashedPw()
}

func main() {
	// enables the usage of generated stuff
	RegisterGeneratedResolver()

	lis, err := net.Listen("tcp", props.listenOn)
	if err != nil {
		panic(err)
	}

	printDebug("listening on " + props.listenOn + " for new connections...")

	// handle connections
	for {
		conn, err := lis.Accept()
		if err != nil {
			printDebug("Error while accepting a connection from " + conn.RemoteAddr().String())
		}

		go handleTCPConnection(conn)
	}

}
