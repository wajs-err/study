//go:build !solution

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"net/http"
	"os"
	"strconv"
	"time"
)

func fillSquare(i int, j int, k int, c color.Color, img *image.RGBA) {
	for x := range k {
		for y := range k {
			img.Set(i*k+x, j*k+y, c)
		}
	}
}

func parseDigitToPic(n int, offset int, k int, img *image.RGBA) {
	var numString string
	switch n {
	case 0:
		numString = Zero
	case 1:
		numString = One
	case 2:
		numString = Two
	case 3:
		numString = Three
	case 4:
		numString = Four
	case 5:
		numString = Five
	case 6:
		numString = Six
	case 7:
		numString = Seven
	case 8:
		numString = Eight
	case 9:
		numString = Nine
	}

	for i := range Height {
		for j := range NumWidth {
			if numString[i*(NumWidth+1)+j] == '1' {
				fillSquare(j+offset, i, k, Cyan, img)
			} else {
				fillSquare(j+offset, i, k, image.White, img)
			}
		}
	}
}

func addColomToPic(offset int, k int, img *image.RGBA) {
	for i := range Height {
		for j := range ColonWidth {
			if Colon[i*(ColonWidth+1)+j] == '1' {
				fillSquare(j+offset, i, k, Cyan, img)
			} else {
				fillSquare(j+offset, i, k, image.White, img)
			}
		}
	}
}

func displayTime(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
	}

	timeString := r.URL.Query().Get("time")
	if timeString == "" {
		timeString = time.Now().Format(time.TimeOnly)
	}
	t, err := time.Parse(time.TimeOnly, timeString)
	if err != nil || len(timeString) != 8 {
		http.Error(w, "wrong time format", http.StatusBadRequest)
	}
	h, m, s := t.Hour(), t.Minute(), t.Second()

	kString := r.URL.Query().Get("k")
	k := 1
	if kString != "" {
		k, err = strconv.Atoi(kString)
		if err != nil || k < 1 || k > 30 {
			http.Error(w, "wrong k format", http.StatusBadRequest)
		}
	}

	img := image.NewRGBA(image.Rect(0, 0, (6*NumWidth+2*ColonWidth)*k, Height*k))
	offset := 0

	parseDigitToPic(h/10, offset, k, img)
	offset += NumWidth
	parseDigitToPic(h%10, offset, k, img)
	offset += NumWidth

	addColomToPic(offset, k, img)
	offset += ColonWidth

	parseDigitToPic(m/10, offset, k, img)
	offset += NumWidth
	parseDigitToPic(m%10, offset, k, img)
	offset += NumWidth

	addColomToPic(offset, k, img)
	offset += ColonWidth

	parseDigitToPic(s/10, offset, k, img)
	offset += NumWidth
	parseDigitToPic(s%10, offset, k, img)

	f, _ := os.Create(fmt.Sprintf("/tmp/%d.%d.%d_%d.png", h, m, s, k))
	_ = png.Encode(f, img)

	w.Header().Set("Content-Type", "image/png")
	w.WriteHeader(http.StatusOK)
	_ = png.Encode(w, img)
}

func main() {
	port := flag.String("port", "8000", "port to listen")
	flag.Parse()

	http.HandleFunc("/", displayTime)
	log.Fatal(http.ListenAndServe("localhost:"+*port, nil))
}
