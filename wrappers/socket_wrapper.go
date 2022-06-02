package wrappers

import (
	"bufio"
	"log"
	"net"
)

var
	clientMap *ClientMap


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

	clientMap = NewClientMap()

	log.Println("Listening on: " + address)

	return l
}

// listenClients
func listenForNewClient(listener net.Listener, addrChan chan string) {
	for {
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

		addr := conn.RemoteAddr().String()

		log.Printf("New client connected: %s\n", addr)

		clientMap.Set(addr, client)
		addrChan <- addr
	}
}

//ListenTcp
func TcpManager(tcpMsg chan string, listener net.Listener) {
	setTcpLog()

	msgBuf := make(chan []byte)
	clientAddr := make(chan string)

	go listenForNewClient(listener, clientAddr)

	for {
		select {
			case clientAddrChan := <- clientAddr:
				go readTcp(msgBuf, clientAddrChan)
			case readChan := <-msgBuf:
				tcpMsg <- string(readChan)
		}
	}
}

func readTcp(msgBuf chan []byte, addr string) {
	setTcpLog()

	client, exists := clientMap.Get(addr)
	if !exists {
		log.Println("Connection to client failed")
		return
	}

	for {
		buf, err := client.reader.ReadBytes('\n')
		if err != nil {
			log.Println("Error reading, disconnecting client: ", err.Error())
			delete(clientMap.clients, client.conn.RemoteAddr().String())
			return
		}

		msgBuf <- buf
	}
}

func WriteTcp(msg string) {

	var n int

	for client := range clientMap.iterate(){
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

	if clientMap.ClientLen() > 0  {
		setTcpLog()
		log.Printf("Wrote %d bytes.\n", n)
	}
}