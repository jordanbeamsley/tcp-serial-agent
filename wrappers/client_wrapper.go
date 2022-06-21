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

type ClientMap struct {
	sync.RWMutex
	clients map[string]Client
}

func NewClientMap() *ClientMap {
	cm := &ClientMap{
		clients: make(map[string]Client),
	}

	return cm
}

func (cm *ClientMap) Set(key string, c Client) {
	cm.Lock()
	defer cm.Unlock()

	cm.clients[key] = c
}

func (cm *ClientMap) Get(key string) (Client, bool) {
	cm.Lock()
	defer cm.Unlock()

	value, ok := cm.clients[key]
	return value, ok
}

func (cm *ClientMap) Delete(key string) {
	cm.Lock()
	defer cm.Unlock()

	delete(cm.clients, key)
}

func (cm *ClientMap) iterate() <- chan Client {
	c := make(chan Client)

	f := func() {
		cm.Lock()
		defer cm.Unlock()
		for _, v := range cm.clients {
			c <- v
		}
		close(c)
	}
	go f()

	return c
}

func (cm *ClientMap) ClientLen() int {
	cm.RLock()
	defer cm.RUnlock()

	return len(cm.clients)
}