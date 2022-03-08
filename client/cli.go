package main

import (
	"fmt"
	"log"
	"net"
	"os"

	"golang.org/x/term"
)

var CLIArgs struct {
	address string
	isHelp  bool
	isDebug bool
}

func printUsage() {
	fmt.Println(
		`Usage: scrtp [-hD] <addr>:<port>

Flags:
-h    Print this page
-D    Print debug info`)
}

// check if the arguments passed are ok
func checkArgs() error {
	var e error = nil

	if len(os.Args) == 1 {
		e = fmt.Errorf("not enough arguments")
	}

	for _, arg := range os.Args {

		switch arg {
		case "-h":
			CLIArgs.isHelp = true
		case "-D":
			CLIArgs.isDebug = true
		}

		if arg[0] == '-' {
			return fmt.Errorf("unknown argument `" + arg + "'")
		} else {
			CLIArgs.address = arg
		}
	}

	return e
}

func askPassword(remoteAddr net.Addr) string {
	fmt.Print("Enter password for " + remoteAddr.String() + ": ")

	pw, err := term.ReadPassword(0)
	if err != nil {
		panic(err)
	}

	return string(pw)
}

// log content if debug is enabled
func printDebug(msg string) {
	if CLIArgs.isDebug {
		log.Println(msg)
	}
}
