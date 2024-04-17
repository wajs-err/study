//go:build !solution

package batcher

import (
	"sync"
	"sync/atomic"

	"gitlab.com/slon/shad-go/batcher/slow"
)

type Batcher struct {
	s        *slow.Value
	last     interface{}
	barrier  chan struct{}
	barrier_ chan struct{}
	running  atomic.Bool
	waiting  atomic.Bool
	wg       sync.WaitGroup
}

func NewBatcher(v *slow.Value) *Batcher {
	return &Batcher{s: v, barrier: make(chan struct{}), barrier_: make(chan struct{})}
}

func (b *Batcher) Load() interface{} {
	if b.waiting.Load() {
		<-b.barrier_
	}
	b.waiting.Store(true)

	if b.running.CompareAndSwap(false, true) {
		defer func() {
			close(b.barrier)
			b.barrier = make(chan struct{})
			b.wg.Wait()
			b.running.Store(false)
			b.waiting.Store(false)
			close(b.barrier_)
			b.barrier_ = make(chan struct{})
		}()
		b.last = b.s.Load()
		return b.last
	}

	b.wg.Add(1)
	defer func() {
		b.wg.Done()
	}()
	<-b.barrier
	return b.last
}
