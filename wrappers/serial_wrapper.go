package wrappers

import (
	"log"

	"github.com/tarm/serial"
)	

func setSerLog() {
	log.SetPrefix("Serial - ")
	log.SetFlags(0)
}

func InitSer(port string, baud int) *serial.Port {
	setSerLog()

	c := &serial.Config{Name: port, Baud: baud}

	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	return s
}

func ListenSer(msg chan string, s *serial.Port) {
	setSerLog()

	buf:= make([]byte, 1024)

	for {
		_, err := s.Read(buf)
		if err != nil {
			log.Println(err)
		}

		msg <- string(buf)
	}
}

func WriteSer(msg string, s *serial.Port) {
	setSerLog()
	
	n, err := s.Write([]byte(msg))
	if err != nil {
		log.Println(err)
		return
	}

	log.Printf("Wrote %d bytes.\n", n)
}