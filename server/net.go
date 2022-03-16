package main

//go:generate msgpackgen

import (
	"net"
	"time"

	"github.com/shamaton/msgpackgen/msgpack"
)

// auth packet
type AuthPkt struct {
	hpw string
	ver string
}

// server reply to auth packet
type AuthRepPkt struct {
	ok      bool
	issue   string
	windows []struct {
		name string
		id   int
	}
}

// client reply with window ID
type WIDPkt struct {
	id int
}

// server window frame
type ServerWinFramePkt struct {
	width  int
	height int
	compfr []byte
}

// client input signal
type ClientInputSigPkt struct {
	source int
	mposx  int
	mposy  int
	keys   []string
}

// buffer size for receiving bytes
const bufferSize int = 1024

// connection timeout
const timeout time.Duration = 5 * time.Second

// UDP connection for sending window frames and input signals
// must be constant after handshake
var udpConn *net.UDPConn

// send a slice of bytes
func sendBytes(conn net.Conn, b []byte) {
	n, err := conn.Write(b)
	if err != nil {
		panic(err)
	}

	if n < len(b) {
		panic("could not send enough bytes")
	}
}

// receive a slice of bytes, reading all the content
func recvBytes(conn net.Conn) []byte {

	total := make([]byte, 0)

	for {
		partial := recvNBytes(conn, bufferSize)

		if len(partial) < bufferSize {
			break
		}

		total = append(total, partial...)
	}

	return total
}

func recvNBytes(conn net.Conn, n int) []byte {
	buffer := make([]byte, n)

	_, err := conn.Read(buffer)
	if err != nil {
		panic(err)
	}

	return buffer
}

func recvAuthPkt(conn net.Conn) AuthPkt {
	b := recvBytes(conn)
	var pkt AuthPkt

	err := msgpack.Unmarshal(b, &pkt)
	if err != nil {
		panic(err)
	}

	return pkt
}

func sendAuthRepPkt(conn net.Conn, pkt AuthRepPkt) {
	b, err := msgpack.Marshal(pkt)
	if err != nil {
		panic(err)
	}

	sendBytes(conn, b)
}

func recvWIDPkt(conn net.Conn) WIDPkt {
	b := recvBytes(conn)
	var pkt WIDPkt

	err := msgpack.Unmarshal(b, &pkt)
	if err != nil {
		panic(err)
	}

	return pkt
}

func sendServerWinFramePkt(conn net.Conn, pkt ServerWinFramePkt) {
	b, err := msgpack.Marshal(pkt)
	if err != nil {
		panic(err)
	}

	sendBytes(conn, b)
}

func recvClientInputSigPkt(conn net.Conn) ClientInputSigPkt {
	b := recvBytes(conn)
	var pkt ClientInputSigPkt

	err := msgpack.Unmarshal(b, &pkt)
	if err != nil {
		panic(err)
	}

	return pkt
}
