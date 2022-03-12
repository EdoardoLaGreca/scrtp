package main

import (
	"fmt"
	"log"
	"os"
)

func printUsage() {
	fmt.Println(`Usage: scrtp-server [-hD]

Flags:
 -h    Print this page
 -D    Enable debug info`)
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
			props.isHelp = true
		case "-D":
			props.isDebug = true
		}

		if arg[0] == '-' {
			return fmt.Errorf("unknown argument `" + arg + "'")
		} else {
			props.listenOn = arg
		}
	}

	return e
}

// log content if debug is enabled
func printDebug(msg string) {
	if props.isDebug {
		log.Println(msg)
	}
}
