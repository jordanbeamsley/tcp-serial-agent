package main

import (
	//"fmt"
	"log"
	"net"

	"github.com/jordanbeamsley/sio-bridge/wrappers"
)

const (
	CONN_HOST = "localhost"
	CONN_PORT = "3333"
	CONN_TYPE = "tcp"

	SER_PORT = "/dev/pts/2"
	SER_BAUD = 9600
)


func main() {
	log.SetPrefix("io-agent - ")
	log.SetFlags(0)

	// Create channels for TCP and serial messages
	tcpMsg := make(chan string)
	serMsg := make(chan string)

	// Init TCP
	var l net.Listener
	address := wrappers.GetAddress(CONN_HOST, CONN_PORT)
	l = wrappers.InitTcp(CONN_TYPE, address)
	//defer l.Close()

	var clients wrappers.ClientSlice

	// Init Serial
	s := wrappers.InitSer(SER_PORT, SER_BAUD)

	// Run listener goroutines
	go wrappers.ListenTcp(tcpMsg, l, &clients)
	go wrappers.ListenSer(serMsg, s)

	for {
		select {
			case tcpMsgChan := <- tcpMsg:
				go wrappers.WriteSer(tcpMsgChan, s)
			case serMsgChan := <- serMsg:
				go wrappers.WriteTcp(serMsgChan, &clients)
		}
	}
}
