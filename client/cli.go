package main

import (
	"fmt"
	"os"
)

// check if the arguments passed are ok
func checkArgs() error {
	var e error

	if len(os.Args) < 2 {
		e = fmt.Errorf("Not enough arguments")
	} else if len(os.Args) > 2 {
		e = fmt.Errorf("Too many arguments, extra arguments will be ignored")
	}

	return e
}
