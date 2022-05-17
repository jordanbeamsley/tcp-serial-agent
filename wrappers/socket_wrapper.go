package wrappers

import (
	"log"
	"net"
)

type Client struct {
	
}

func setTcpLog() {
	log.SetPrefix("TCP - ")
	log.SetFlags(0)
}

func GetAddress(host, port string) string {
	return host + ":" + port
}

func InitTcp(network string, address string) net.Listener {
	setTcpLog()

	l, err := net.Listen(network, address)
	if err != nil {
		log.Fatal(err)
	}

	log.Println("Listening on: " + address)

	return l
}

func ListenTcp(tcpMsg chan string, listener net.Listener) {
	setTcpLog()

	msgBuf := make(chan []byte)

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Println(err)
		}

		go readTcp(msgBuf, conn)

		select {
			case readChan := <-msgBuf:
				tcpMsg <- string(readChan)
		}
	}
}

func readTcp(msgBuf chan []byte, conn net.Conn) (msg []byte) {
	setTcpLog()

	buf := make([]byte, 1024)

	_, err := conn.Read(buf)
	if err != nil {
		log.Println("Error reading", err.Error())
	}

	msgBuf <- buf
	conn.Close()

	return
}

func WriteTcp(msg string, conn net.Conn) {
	n, err := conn.Write([]byte(msg))
	if err != nil {
		log.Println(err)
		return
	}

	log.Printf("Wrote %d bytes.\n", n)
}