//go:build !solution

package httpgauge

import (
	"fmt"
	"net/http"
	"sort"
	"sync"

	"github.com/go-chi/chi/v5"
)

type Gauge struct {
	mu sync.Mutex
	m  map[string]int
}

func New() *Gauge {
	return &Gauge{mu: sync.Mutex{}, m: make(map[string]int)}
}

func (g *Gauge) Snapshot() map[string]int {
	s := make(map[string]int)
	g.mu.Lock()
	defer g.mu.Unlock()
	for k, v := range g.m {
		s[k] = v
	}
	return s
}

// ServeHTTP returns accumulated statistics in text format ordered by pattern.
//
// For example:
//
//	/a 10
//	/b 5
//	/c/{id} 7
func (g *Gauge) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	s := g.Snapshot()
	keys := make([]string, 0, len(s))

	for k := range s {
		keys = append(keys, k)
	}
	sort.Slice(keys, func(i, j int) bool {
		return keys[i] < keys[j]
	})

	for _, k := range keys {
		_, _ = w.Write([]byte(k))
		_, _ = w.Write([]byte(" "))
		_, _ = w.Write([]byte(fmt.Sprintf("%d\n", s[k])))
	}
}

func (g *Gauge) Wrap(next http.Handler) http.Handler {
	fn := func(w http.ResponseWriter, r *http.Request) {
		defer func() {
			rec := recover()
			path := chi.RouteContext(r.Context()).RoutePattern()
			g.mu.Lock()
			g.m[path] += 1
			g.mu.Unlock()
			if rec != nil {
				panic(rec)
			}
		}()
		next.ServeHTTP(w, r)
	}
	return http.HandlerFunc(fn)
}
