package wrappers

import (
	"bufio"
	"log"
	"net"
)

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

func listenClients(listener net.Listener, clients *ClientSlice, newClient chan Client) {
	for {
		//TODO: Needs to be moved to a routine. Blocking after first connection
		conn, err := listener.Accept()
		if err != nil {
			log.Println(err)
			continue
		}

		client := Client{
			reader: bufio.NewReader(conn),
			writer: bufio.NewWriter(conn),
			conn: conn,
		}

		log.Printf("New client connected: %s\n", conn.RemoteAddr().String())

		clients.appendClient(client)
		newClient <- client
	}
}

func ListenTcp(tcpMsg chan string, listener net.Listener, clients *ClientSlice) {
	setTcpLog()

	msgBuf := make(chan []byte)
	newClient := make(chan Client)

	go listenClients(listener, clients, newClient)

	for {
		select {
			case newClientChan := <- newClient:
				go readTcp(msgBuf, newClientChan)
			case readChan := <-msgBuf:
				tcpMsg <- string(readChan)
		}
	}
}

func readTcp(msgBuf chan []byte, client Client) {
	setTcpLog()


	for {
		buf, err := client.reader.ReadBytes('\n')
		if err != nil {
			log.Println("Error reading", err.Error())
		}

		msgBuf <- buf
	}
}

func WriteTcp(msg string, cs *ClientSlice) {

	var n int

	for client := range cs.iterateClients(){
		var err error
		n, err = client.writer.WriteString(msg)
		if err != nil {
			log.Println(err)
		}
		err = client.writer.Flush()
		if err != nil {
			log.Println(err)
		}
	}

	setTcpLog()
	log.Printf("Wrote %d bytes.\n", n)
}