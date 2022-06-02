package main

import (
	"log"

	"github.com/jordanbeamsley/sio-bridge/wrappers"
)

const (
	CONN_HOST = "localhost"
	CONN_PORT = "3333"
	CONN_TYPE = "tcp"

	SER_PORT = "/dev/ttyUSB0"
	SER_BAUD = 9600
)


func main() {
	log.SetPrefix("io-agent - ")
	log.SetFlags(0)

	// Create channels for TCP and serial messages
	tcpMsg := make(chan string)
	serMsg := make(chan string)


	//Init TCP
	l := wrappers.InitTcp(CONN_TYPE, wrappers.GetAddress(CONN_HOST, CONN_PORT))

	// Init Serial
	s := wrappers.InitSer(SER_PORT, SER_BAUD)

	// Run listener goroutines
	go wrappers.TcpManager(tcpMsg, l)
	go wrappers.ListenSer(serMsg, s)

	for {
		select {
			case tcpMsgChan := <- tcpMsg:
				go wrappers.WriteSer(tcpMsgChan, s)
			case serMsgChan := <- serMsg:
				go wrappers.WriteTcp(serMsgChan)
		}
	}
}
