//go:build !solution

package batcher

import (
	"sync/atomic"

	"gitlab.com/slon/shad-go/batcher/slow"
)

type Batcher struct {
    s       *slow.Value
    last    interface{}
    barrier chan struct{}
    running atomic.Bool
    gen     atomic.Uint64
}

func NewBatcher(v *slow.Value) *Batcher {
    return &Batcher{s: v, last: nil, barrier: make(chan struct{})}
}

func (b *Batcher) Load() interface{} {
    if b.running.CompareAndSwap(false, true) {
        b.last = b.s.Load()
        b.running.Store(false)
        close(b.barrier)
        b.barrier = make(chan struct{})
        return b.last
    }

    select {
    case <-b.barrier:
        return b.last
    }
}

func (b *Batcher) Store(v interface{}) {
    b.gen.Add(1)
    b.s.Store(v)
}
