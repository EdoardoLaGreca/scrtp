package main

//go:generate msgpackgen

import (
	"net"

	"github.com/shamaton/msgpackgen/msgpack"
)

// buffer size for receiving bytes
const bufferSize int = 1024

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

type ServerWinFramePkt struct {
	width  int
	height int
	compfr []int
}

type ClientInputSigPkt struct {
	source  int
	ispress bool
	mposx   int
	mposy   int
	keys    []string
}

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
	return recvNBytes(conn, bufferSize)
}

func recvNBytes(conn net.Conn, n int) []byte {
	buffer := make([]byte, n)

	n, err := conn.Read(buffer)
	if err != nil {
		panic(err)
	}

	return buffer
}

func sendAuthPkt(conn net.Conn, pkt AuthPkt) {
	b, err := msgpack.Marshal(pkt)
	if err != nil {
		panic(err)
	}

	sendBytes(conn, b)
}

func recvAuthRepPkt(conn net.Conn) AuthRepPkt {
	b := recvBytes(conn)
	var pkt AuthRepPkt

	err := msgpack.Unmarshal(b, &pkt)
	if err != nil {
		panic(err)
	}

	return pkt
}

func sendWIDPkt(conn net.Conn, pkt WIDPkt) {
	b, err := msgpack.Marshal(pkt)
	if err != nil {
		panic(err)
	}

	sendBytes(conn, b)
}

func recvServerWinFramePkt(conn net.Conn) ServerWinFramePkt {
	b := recvBytes(conn)
	var pkt ServerWinFramePkt

	err := msgpack.Unmarshal(b, &pkt)
	if err != nil {
		panic(err)
	}

	return pkt
}

func sendClientInputSigPkt(conn net.Conn, pkt ClientInputSigPkt) {
	b, err := msgpack.Marshal(pkt)
	if err != nil {
		panic(err)
	}

	sendBytes(conn, b)
}