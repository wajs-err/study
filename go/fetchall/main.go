//go:build !solution

package main

import (
	"fmt"
	"net/http"
	"os"
)

func FetchURL(url string, ch chan<- string) {
	resp, err := http.Get(url)
	if err != nil {
		ch <- fmt.Sprintf("%s\tERROR", url)
		return
	}
	defer resp.Body.Close()
	ch <- fmt.Sprintf("%s\t%s", url, resp.Status)
}

func main() {
	messages := make(chan string)
	for _, url := range os.Args[1:] {
		go FetchURL(url, messages)
	}
	for i := 0; i < len(os.Args)-1; i += 1 {
		fmt.Println(<-messages)
	}
}
