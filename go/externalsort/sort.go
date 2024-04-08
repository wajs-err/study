//go:build !solution

package externalsort

import (
	"container/heap"
	"io"
	"os"
)

func findEndOfLine(buff []byte) int {
	for i, b := range buff {
		if b == '\n' {
			return i
		}
	}
	return -1
}

type internalLineReader struct {
	r    io.Reader
	buff []byte
}

func NewReader(r io.Reader) LineReader {
	return &internalLineReader{r: r, buff: make([]byte, 0)}
}

func (lr *internalLineReader) ReadLine() (string, error) {
	endOfLine := findEndOfLine(lr.buff)
	for endOfLine == -1 {
		b := make([]byte, 1024)
		l, err := lr.r.Read(b)
		if l != 0 {
			lr.buff = append(lr.buff, b[:l]...)
		}
		if err != nil {
			s := string(lr.buff)
			return s, err
		}
		endOfLine = findEndOfLine(lr.buff)
	}
	s := string(lr.buff[:endOfLine])
	lr.buff = lr.buff[endOfLine+1:]
	return s, nil
}

type internalLineWriter struct {
	w io.Writer
}

func NewWriter(w io.Writer) LineWriter {
	return &internalLineWriter{w: w}
}

func (lw *internalLineWriter) Write(l string) error {
	if _, err := lw.w.Write(append([]byte(l), '\n')); err != nil {
		return err
	}
	return nil
}

func Merge(w LineWriter, readers ...LineReader) error {
	if len(readers) == 0 {
		return nil
	}

	lines := make([]string, len(readers))
	empty := make([]bool, len(readers))
	emptyCount := 0
	for i, r := range readers {
		l, err := r.ReadLine()
		lines[i] = l
		if err != nil {
			empty[i] = true
			if l == "" {
				emptyCount++
			}
		}
	}

	for emptyCount != len(readers) {
		min := lines[0]
		minIndex := 0
		for i, l := range lines {
			if !empty[i] || l != "" {
				min = l
				minIndex = i
				break
			}
		}

		for i, l := range lines {
			if empty[i] && l == "" {
				continue
			}
			if l < min {
				min = l
				minIndex = i
			}
		}

		if err := w.Write(min); err != nil {
			return err
		}

		if empty[minIndex] {
			emptyCount++
			lines[minIndex] = ""
			continue
		}

		l, err := readers[minIndex].ReadLine()
		lines[minIndex] = l
		if err != nil {
			empty[minIndex] = true
			if l == "" {
				emptyCount++
			}
		}
	}

	return nil
}

type stringHeap []string

func (h stringHeap) Len() int {
	return len(h)
}

func (h stringHeap) Less(i, j int) bool {
	return h[i] < h[j]
}

func (h stringHeap) Swap(i, j int) {
	h[i], h[j] = h[j], h[i]
}

func (h *stringHeap) Push(x any) {
	*h = append(*h, x.(string))
}

func (h *stringHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[0 : n-1]
	return x
}

func Sort(w io.Writer, in ...string) error {
	readers := make([]LineReader, len(in))
	for i, path := range in {
		file, err := os.OpenFile(path, os.O_RDWR, 0777)
		if err != nil {
			return err
		}
		defer file.Close()

		reader := NewReader(file)
		lines := make([]string, 0)
		for {
			l, err := reader.ReadLine()
			if err == nil || l != "" {
				lines = append(lines, l)
			}
			if err != nil {
				break
			}
		}

		h := stringHeap(lines)
		heap.Init(&h)

		file.Seek(0, io.SeekStart)
		w := NewWriter(file)
		for h.Len() > 0 {
			w.Write(heap.Pop(&h).(string))
		}

		file.Sync()
		file.Seek(0, io.SeekStart)

		readers[i] = NewReader(file)
	}

	if err := Merge(NewWriter(w), readers...); err != nil {
		return err
	}
	return nil
}
