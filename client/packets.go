package client

import (
	"crypto/cipher"
	"net"
)

// send an encrypted byte slice to server
func sendEnc(conn net.Conn, cipher cipher.Block, content []byte) error {
	// encrypted content
	encContent := make([]byte, keySize)

	cipher.Encrypt(encContent, content)

	_, err := conn.Write(encContent)

	if err != nil {
		return err
	}

	return nil
}

// receive all the content decrypted
func receiveAllDec(conn net.Conn, cipher cipher.Block) ([]byte, error) {
	content := make([]byte, 0)

	for {
		buffer := make([]byte, 512) // buffer of half kilo
		n, err := conn.Read(buffer)

		if err != nil {
			return nil, err
		}

		buffer = buffer[:n] // resize buffer
		content = append(content, buffer...)

		if n < len(buffer) {
			break
		}
	}

	decContent := make([]byte, 0)
	cipher.Decrypt(content, decContent)

	return decContent, nil
}

// receive N bytes decrypted
func receiveNDec(conn net.Conn, cipher cipher.Block, n int) ([]byte, error) {
	content := make([]byte, n)

	_, err := conn.Read(content)

	if err != nil {
		return nil, err
	}

	decContent := make([]byte, n)
	cipher.Decrypt(content, decContent)

	return decContent, nil
}

// build auth packet
func buildAuthPkt(key []byte, MACaddr, protoVer, imgQuality string) []byte {

	var pkt []byte
	
	pkt = append(pkt, (string(key) + "\n" + MACaddr + "\n" + protoVer + "\n" + imgQuality + "\x00")...)
	
	return pkt
}
