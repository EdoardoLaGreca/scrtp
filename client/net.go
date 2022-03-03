package main

//go:generate msgpackgen

import (
	_ "github.com/shamaton/msgpackgen"
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

/*
// send a slice of bytes
func sendBytes(b []byte) {

}

// receive a slice of bytes, reading all the content
func recvBytes() []byte {

}

func sendAuthPkt(pkt AuthPkt) {

}

func recvAuthRepPkt() AuthRepPkt {

}

func sendWIDPkt(pkt WIDPkt) {

}

func recvServerWinFramePkt() ServerWinFramePkt {

}

func sendClientInputSigPkt(pkg ClientInputSigPkt) {

}
*/
