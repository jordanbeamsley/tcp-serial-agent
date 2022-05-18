package wrappers

import (
	"bufio"
	"log"

	"github.com/tarm/serial"
)	

type serialInterface struct {
	reader *bufio.Reader
	writer *bufio.Writer
	port *serial.Port
}

func setSerLog() {
	log.SetPrefix("Serial - ")
	log.SetFlags(0)
}

func InitSer(port string, baud int) *serialInterface {
	setSerLog()

	c := &serial.Config{Name: port, Baud: baud}

	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	serial := serialInterface{
		reader: bufio.NewReader(s),
		writer: bufio.NewWriter(s),
		port: s,
	}

	return &serial
}

func ListenSer(msg chan string, s *serialInterface) {
	setSerLog()

	for {
		buf, err := s.reader.ReadBytes('\n')
		if err != nil {
			log.Println(err)
		}

		msg <- string(buf)
	}
}

func WriteSer(msg string, s *serialInterface) {
	setSerLog()
	
	n, err := s.writer.WriteString(msg)
	if err != nil {
		log.Println(err)
		return
	}
	err = s.writer.Flush()
	if err != nil {
		log.Println(err)
	}

	log.Printf("Wrote %d bytes.\n", n)
}