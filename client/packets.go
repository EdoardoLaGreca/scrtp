package client

import (
	"crypto/cipher"
	"encoding/binary"
	"net"

	"github.com/DataDog/zstd"
)

type WindowFrame struct {
	x, y          uint
	width, height uint
	winFrame      []byte
}

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

// build auth packet
func buildAuthPkt(key []byte, MACaddr, protoVer, imgQuality string) []byte {
	pkt := make([]byte, 0)

	pkt = append(pkt, (string(key) + "\n" + MACaddr + "\n" + protoVer + "\n" + imgQuality + "\x00")...)

	return pkt
}

func receiveWinFrame(conn net.Conn, cipher cipher.Block) (*WindowFrame, error) {
	pkt, err := receiveAllDec(conn, cipher)

	if err != nil {
		return nil, err
	}

	winFrameDec, err := zstd.Decompress(nil, pkt[16:])

	if err != nil {
		return nil, err
	}

	wf := WindowFrame{
		x:        uint(binary.BigEndian.Uint32(pkt[0:4])),
		y:        uint(binary.BigEndian.Uint32(pkt[4:8])),
		width:    uint(binary.BigEndian.Uint32(pkt[8:12])),
		height:   uint(binary.BigEndian.Uint32(pkt[12:16])),
		winFrame: winFrameDec,
	}

	return &wf, nil
}

// send input signal to server
func sendInputSignal(conn net.Conn, cipher cipher.Block, t int8, val1, val2 int32) error {
	pkt := make([]byte, 0)
	tBytes := byte(t)
	val1Bytes := make([]byte, 4)
	val2Bytes := make([]byte, 4)

	binary.BigEndian.PutUint32(val1Bytes, uint32(val1))
	binary.BigEndian.PutUint32(val2Bytes, uint32(val2))

	// fill packet
	pkt = append(pkt, tBytes)
	pkt = append(pkt, val1Bytes...)
	pkt = append(pkt, val2Bytes...)

	err := sendEnc(conn, cipher, pkt)

	return err
}
