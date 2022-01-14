package client

import (
	"crypto/cipher"
	"log"
	"net"
)

// send a byte slice to server
func send(conn net.Conn, cipher cipher.Block, content []byte) {
	// encrypted content
	encContent := make([]byte, keySize)

	cipher.Encrypt(encContent, content)

	_, err := conn.Write(encContent)

	if err != nil {
		log.Fatalln("An error occurred while sending a message to the server:", err)
	}
}

// build auth packet
func buildAuthPkt(key []byte, MACaddr, protoVer, imgQuality string) []byte {

	var pkt []byte
	
	pkt = append(pkt, (string(key) + "\n" + MACaddr + "\n" + protoVer + "\n" + imgQuality + "\x00")...)
	
	return pkt
}
