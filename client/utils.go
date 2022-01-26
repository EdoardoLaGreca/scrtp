package client

import (
	"fmt"
	"net"
	"os"

	"github.com/go-gl/glfw/v3.3/glfw"
)

func createWindow(width, height int) (*glfw.Window, error) {
	err := glfw.Init()

	if err != nil {
		return nil, err
	}

	defer glfw.Terminate()

	window, err := glfw.CreateWindow(width, height, "scrtp", nil, nil)

	if err != nil {
		return nil, err
	}

	window.MakeContextCurrent()

	return window, nil
}

// get current machine's MAC address
func getMACaddr(netname, address string) (string, error) {
	ifs, err := net.Interfaces()

	if err != nil {
		return "", err
	}

	var iface net.Interface

	// find the interface in use among all the others
	for _, ifc := range ifs {

		if ifc.Flags&net.FlagLoopback == 0 && ifc.Flags&net.FlagUp != 0 {
			addrs, err := ifc.Addrs()

			if err != nil {
				continue
			}

			addrFound := false

			for _, addr := range addrs {
				if addr.Network() == netname && addr.String() == address {
					addrFound = true
					break
				}
			}

			if addrFound {
				iface = ifc
				break
			}
		}
	}

	return iface.HardwareAddr.String(), nil
}

// read the key or exit if an error occurred
func readKey() ([]byte, error) {
	homeDir, err := os.UserHomeDir()

	if err != nil {
		return nil, fmt.Errorf("unable to get the home directory path: %w", err)
	}

	keyDir := homeDir + "/.config/scrtp"
	keyFile, err := os.Open(keyDir + "/aes_key")

	if err != nil {
		return nil, fmt.Errorf("no key found in %s: %w", keyDir, err)
	}

	key := make([]byte, keySize)

	keyLen, err := keyFile.Read(key)

	if err != nil {
		return nil, err
	}

	if keyLen != keySize {
		return nil, fmt.Errorf("the key length is not 128 bit")
	}

	return key, nil
}

func updateWindow(window *glfw.Window, part WindowFrame) error {
	return nil
}
