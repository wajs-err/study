//go:build !solution

package fileleak

import (
	"fmt"
	"os"
	"path/filepath"
)

type testingT interface {
	Errorf(msg string, args ...interface{})
	Cleanup(func())
}

func VerifyNone(t testingT) {
	files, err := os.ReadDir("/proc/self/fd")
	if err != nil {
		panic(err.Error())
	}
	symlinksCount := make(map[string]int)
	for _, file := range files {
		path := filepath.Join("/proc/self/fd", file.Name())
		link, err := os.Readlink(path)
		if err == nil {
			symlinksCount[link]++
		}
	}
	t.Cleanup(func() {
		newFiles, err := os.ReadDir("/proc/self/fd")
		if err != nil {
			panic(err.Error())
		}
		newSymlinksCount := make(map[string]int)
		for _, file := range newFiles {
			path := filepath.Join("/proc/self/fd", file.Name())
			link, err := os.Readlink(path)
			if err != nil {
				continue
			}
			newSymlinksCount[link]++
		}
		for k, v := range newSymlinksCount {
			if u, ok := symlinksCount[k]; !ok || u < v {
				t.Errorf(fmt.Sprintf("problem file is %s", k))
				return
			}
		}
	})
}
