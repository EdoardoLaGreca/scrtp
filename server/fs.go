package main

import (
	"fmt"
	"os"
	"strings"

	"golang.org/x/crypto/sha3"
)

// read config file stored in path
func readConfig() ([]property, error) {
	b, err := os.ReadFile("/etc/scrtp/server_config")
	if err != nil {
		return nil, err
	}

	fileContent := string(b)
	props := make([]property, 0)

	var key, value string
	isKey := true

	// parse file to get keys and values
	for i := 0; i < len(fileContent); i++ {
		switch fileContent[i] {
		case '\n':
			key = strings.TrimSpace(key)

			if strings.Contains(key, " ") {
				return nil, fmt.Errorf("key contains one or more spaces")
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

// read password and hash it
func getHashedPw() string {
	b, err := os.ReadFile("/etc/scrtp/server_pw")
	if err != nil {
		panic(err)
	}

	sumArray := sha3.Sum256(b)
	sumSlice := sumArray[:]

	return string(sumSlice)
}
