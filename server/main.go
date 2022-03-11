package main

import (
	"fmt"
	"net"
	"os"
	"strings"
)

type property struct {
	key   string
	value string
}

var settings struct {
	listenOn string
	isHelp   bool
	isDebug  bool
}

func readConfig(path string) ([]property, error) {
	b, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	fileContent := string(b)
	props := make([]property, 0)

	var key, value string
	isKey := true

	for i := 0; i < len(fileContent); i++ {
		switch fileContent[i] {
		case '\n':
			key = strings.TrimSpace(key)

			if strings.Contains(key, " ") {
				return nil, fmt.Errorf("key \"%s\" contains one or more spaces")
			}

			if key != "" && value != "" {
				props = append(props, property{key: key, value: value})
			}
			key = ""
			value = ""
			isKey = true
		case '=':
			isKey = false
		case ' ':
			continue
		case '#':
			// skip to the next newline
			for ; fileContent[i+1] != '\n'; i++ {
				continue
			}
		default:
			// add character to string
			if isKey {
				key = fmt.Sprintf("%s%c", key, fileContent[i])
			} else {
				value = fmt.Sprintf("%s%c", value, fileContent[i])
			}
		}
	}

	return props, nil
}

func handleConnection(conn net.Conn) {
	//TODO
}

func main() {
	err := checkArgs()
	if err != nil {
		printUsage()
		panic(err)
	}

	// enables the usage of generated stuff
	RegisterGeneratedResolver()

	lis, err := net.Listen("tcp", settings.listenOn)
	if err != nil {
		panic(err)
	}

	printDebug("listening on " + settings.listenOn + "...")

	for {
		conn, err := lis.Accept()
		if err != nil {
			printDebug("Error while accepting a connection from " + conn.RemoteAddr().String())
		}

		go handleConnection(conn)
	}

}
