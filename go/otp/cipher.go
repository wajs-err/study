//go:build !solution

package otp

import (
	"io"
)

type StreamCipherReader struct {
	r    io.Reader
	prng io.Reader
}

func (r *StreamCipherReader) Read(p []byte) (n int, err error) {
	q := make([]byte, len(p))
	n, err = r.r.Read(q)
	random := make([]byte, n)
	_, _ = r.prng.Read(random)
	for i := range random {
		p[i] = q[i] ^ random[i]
	}
	return n, err
}

type StreamCipherWriter struct {
	w    io.Writer
	prng io.Reader
}

func (w *StreamCipherWriter) Write(p []byte) (n int, err error) {
	c := make([]byte, len(p))
	copy(c, p)
	random := make([]byte, len(p))
	_, _ = w.prng.Read(random)
	for i := range random {
		c[i] ^= random[i]
	}
	n, err = w.w.Write(c)
	return n, err
}

func NewReader(r io.Reader, prng io.Reader) io.Reader {
	return &StreamCipherReader{r, prng}
}

func NewWriter(w io.Writer, prng io.Reader) io.Writer {
	return &StreamCipherWriter{w, prng}
}
