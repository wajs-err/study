//go:build !solution

package main

import (
	"crypto/rand"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net/http"
	"strings"
	"sync"
)

type URL struct {
	URL string `json:"url"`
}

var URLByKey sync.Map
var KeyByURL sync.Map

func shorten(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
	}

	url := &URL{}
	err := json.NewDecoder(r.Body).Decode(&url)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	var key string
	if k, ok := KeyByURL.Load(url.URL); ok {
		key = k.(string)
	} else {
		shorten := make([]byte, 6)
		_, _ = rand.Read(shorten)
		key = fmt.Sprintf("%x", shorten)

		URLByKey.Store(key, url.URL)
		KeyByURL.Store(url.URL, key)
		http.HandleFunc("/go/"+key, get)
	}

	result := map[string]string{
		"url": url.URL,
		"key": key,
	}
	jsonData, err := json.Marshal(result)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write(jsonData)
}

func get(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
	}

	path := strings.Split(r.URL.Path, "/")
	key := path[len(path)-1]
	url, ok := URLByKey.Load(key)
	if !ok {
		http.Error(w, "not found", http.StatusNotFound)
	}

	w.Header().Set("Location", url.(string))
	w.WriteHeader(http.StatusFound)
}

func main() {
	port := flag.String("port", "8000", "port to listen")
	flag.Parse()

	http.HandleFunc("/shorten", shorten)
	http.HandleFunc("/go", get)
	log.Fatal(http.ListenAndServe("localhost:"+*port, nil))
}
