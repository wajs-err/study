//go:build !solution

package main

import (
	"fmt"
	"os"
	s "strings"
)

func Check(e error) {
	if e != nil {
		panic(e)
	}
}

func main() {
	m := make(map[string]int)
	for _, file := range os.Args[1:] {
		dat, err := os.ReadFile(file)
		Check(err)
		for _, str := range s.Split(string(dat), "\n") {
			m[str] += 1
		}
	}
	for k, v := range m {
		if v >= 2 {
			fmt.Printf("%d\t%s\n", v, k)
		}
	}
}
