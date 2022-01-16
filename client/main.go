package client

import (
	"crypto/aes"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

// key size is 128 bit
const keySize int = 128/8;

// read the key or exit if an error occurred
func readKey() ([]byte, error) {
	homeDir, err := os.UserHomeDir()

	if err != nil {
		return nil, fmt.Errorf("Unable to get the home directory path: %w", err)
	}

	keyDir := homeDir + "/.config/scrtp"
	keyFile, err := os.Open(keyDir + "/aes_key")

	if err != nil {
		return nil, fmt.Errorf("No key found in %s: %w", keyDir, err)
	}

	key := make([]byte, keySize)

	keyLen, err := keyFile.Read(key)

	if err != nil {
		return nil, err
	}

	if keyLen != keySize {
		return nil, fmt.Errorf("The key length is not 128 bit")
	}

	return key, nil
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

// TODO: better organization of code
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

	key, err := readKey()

	if err != nil {
		log.Fatalln("An error occurred while reading the key:", err)
	}

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
	sendEnc(conn, cipher, buildAuthPkt(key, MACaddr, "1.0", "0"))

	// step 2: read the reply
	reply := make([]byte, 2)
	var restOfPkt string
	
	if _, err := conn.Read(reply); err != nil {
		log.Fatalln("An error occurred while trying to read the server's reply:", err)
	}

	if string(reply) != "OK" && string(reply) != "NO" {
		log.Fatalln("The remote server sent a malformed reply:", reply)
	}
	
	// read the rest of the packet
	bytes, err := receiveAllDec(conn, cipher)

	if err != nil {
		log.Fatalln("An error occurred while trying to read the server's reply:", err)
	}

	restOfPkt = string(bytes)

	if string(reply) == "NO" {
		// restOfPkt contains the issue
		log.Fatalln("The remote server refused the request:", restOfPkt)
	}

	// restOfPkt contains the list of windows
	windows := strings.Split(restOfPkt, "\n")

	// add entry "Entire desktop" as first element
	windows = append(append(windows[:0], "Entire desktop"), windows...)

	fmt.Println("Available windows:")
	for n, w := range windows {
		fmt.Printf("%d - %w\n", n, w)
	}

	var chosenWin int

	fmt.Print("Select one by its number: ")
	fmt.Scanf("%d", &chosenWin)

	// step 3: send the chosen window
	if chosenWin == 0 {
		sendEnc(conn, cipher, []byte("0"))
	} else {
		sendEnc(conn, cipher, []byte(windows[chosenWin]))
	}

	// step 4: get frames
	// continue here
}
