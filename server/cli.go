package main

import (
	"fmt"
	"log"
	"os"
)

func printUsage() {
	fmt.Println(`Usage: scrtp-server [-hD] <config_file>

<config_file> is the configuration file. By default, scrtp-server reads it from
/etc/scrtp/config_server

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
			settings.isHelp = true
		case "-D":
			settings.isDebug = true
		}

		if arg[0] == '-' {
			return fmt.Errorf("unknown argument `" + arg + "'")
		} else {
			settings.listenOn = arg
		}
	}

	return e
}

// log content if debug is enabled
func printDebug(msg string) {
	if settings.isDebug {
		log.Println(msg)
	}
}
