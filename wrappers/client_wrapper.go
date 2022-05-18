package wrappers

import (
	"bufio"
	"net"
	"sync"
)

type Client struct {
	reader *bufio.Reader
	writer *bufio.Writer
	conn net.Conn
}

type ClientSlice struct {
	sync.RWMutex
	clients []Client
} 

func (cs *ClientSlice) appendClient(c Client) {
	cs.Lock()
	defer cs.Unlock()

	cs.clients = append(cs.clients, c)
}

func (cs *ClientSlice) iterateClients() <- chan Client {
	c := make(chan Client)

	f := func() {
		cs.Lock()
		defer cs.Unlock()
		for _, value := range cs.clients {
			c <- value
		}
		close(c)
	}
	go f()

	return c
}

func (cs *ClientSlice) ClientLen() int {
	cs.RLock()
	defer cs.RUnlock()

	return len(cs.clients)
}