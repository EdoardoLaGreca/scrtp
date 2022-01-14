package client

import (
	"crypto/aes"
	"fmt"
	"log"
	"net"
	"os"
	"time"
)

// key size is 128 bit
const keySize int = 128/8;

// read the key or exit if an error occurred
func readKey() []byte {
	homeDir, err := os.UserHomeDir()

	if err != nil {
		log.Fatalln("Unable to get the home directory path:", err)
	}

	keyDir := homeDir + "/.config/scrtp"
	keyFile, err := os.Open(keyDir + "/aes_key")

	if err != nil {
		log.Fatalln("No key found in", keyDir + ":", err)
	}

	key := make([]byte, keySize)

	keyLen, err := keyFile.Read(key)

	if err != nil {
		log.Fatalln("An error occurred while reading the key:", err)
	}

	if keyLen != keySize {
		log.Fatalln("The key length is not 128 bit")
	}

	return key
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
	
		if ifc.Flags & net.FlagLoopback == 0 && ifc.Flags & net.FlagUp != 0 {
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

			if addrFound == true {
				iface = ifc
				break
			}
		}
	}

	return iface.HardwareAddr.String(), nil
}

func main() {
	if len(os.Args) != 3 {
		// usage: scrtp <ip> <port>
		fmt.Println("Usage: scrtp <ip> <port>")
		os.Exit(1)
	}

	ip, port := os.Args[1], os.Args[2]
	address := fmt.Sprintf("%s:%s", ip, port)

	log.Println("Scrtp client started")
	log.Println("Getting the current user's AES key...")

	key := readKey()

	log.Println("Creating a block cipher using the AES key...")

	cipher, err := aes.NewCipher(key)

	if err != nil {
		log.Fatalln("An error occurred while creating the block cipher:", err)
	}

	log.Println("Establishing a TCP connection...")

	conn, err := net.DialTimeout("tcp", address, time.Second * 5)
	defer conn.Close()

	if err != nil {
		log.Fatalln("An error occurred while trying to connect to", "\"" + address + "\":", err)
	}

	log.Println("Getting the current machine's MAC address...")

	MACaddr, err := getMACaddr("tcp", address)

	if err != nil {
		log.Fatalln("An error occurred while trying to get the MAC address:", err)
	}

	// step 1: send auth packet
	log.Println("Sending the authentication packet...")
	send(conn, cipher, buildAuthPkt(key, MACaddr, "1.0", ""))

	// step 2: read the reply
	reply := make([]byte, 2)
	windowList := make([][2]string, 0)

	if _, err := conn.Read(reply); err != nil {
		log.Fatalln("An error occurred while trying to read the server's reply:", err)
	}

	for {
		buffer := // continue here
	}
	
}
