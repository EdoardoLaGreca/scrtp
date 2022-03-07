package main

import (
	"fmt"
	"net"
	"os"

	"golang.org/x/term"
)

// check if the arguments passed are ok
func checkArgs() error {
	var e error = nil

	if len(os.Args) < 2 {
		e = fmt.Errorf("not enough arguments")
	} else if len(os.Args) > 2 {
		e = fmt.Errorf("too many arguments, extra arguments will be ignored")
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
