package main

import (
	"log"
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

// check if the program has enough privilege to access the /etc directory
func checkEtc() bool {
	_, err := os.Open("/etc")

	return err != nil
}

func handleConnection(conn net.Conn) {
	printDebug("!! new connection from " + conn.RemoteAddr().String() + ", waiting for an authentication packet...")

	authPkt := recvAuthPkt(conn)

	printDebug("authentication packet received")

	var reply AuthRepPkt

	if authPkt.hpw != props.hashedPassword {
		printDebug("login from " + conn.RemoteAddr().String() + " failed becuse of wrong password")
		reply = AuthRepPkt{
			ok:    false,
			issue: "wrong password",
		}

		conn.Close()
		return
	}

	printDebug("login from " + conn.RemoteAddr().String() + " successful")

	// ...

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

func init() {
	fillProps()
}

func main() {
	// enables the usage of generated stuff
	RegisterGeneratedResolver()

	lis, err := net.Listen("tcp", props.listenOn)
	if err != nil {
		panic(err)
	}

	printDebug("listening on " + props.listenOn + " for new connections...")

	for {
		conn, err := lis.Accept()
		if err != nil {
			printDebug("Error while accepting a connection from " + conn.RemoteAddr().String())
		}

		go handleConnection(conn)
	}
}
