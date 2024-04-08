//go:build !solution

package tparallel

import "sync"

type T struct {
	next       *T
	canRun     chan struct{}
	isParallel bool
	barrier    chan struct{}
	wg         *sync.WaitGroup
}

func newT(wg *sync.WaitGroup, barrier chan struct{}) *T {
	return &T{canRun: make(chan struct{}, 1), isParallel: false, wg: wg, barrier: barrier}
}

func runTest(t *T, f func(t *T)) {
	<-t.canRun
	f(t)
	if !t.isParallel {
		t.next.canRun <- struct{}{}
	} else {
		t.next.wg.Done()
	}
}

func (t *T) Parallel() {
	t.isParallel = true
	t.next.wg.Add(1)
	t.next.canRun <- struct{}{}
	<-t.next.barrier
}

func (t *T) Run(subtest func(*T)) {
	wg := &sync.WaitGroup{}
	barrier := make(chan struct{}, 1)
	child := newT(wg, barrier)
	child.next = t
	go runTest(child, subtest)
	child.canRun <- struct{}{}
	<-t.canRun
	close(barrier)
	wg.Wait()
}

func Run(topTests []func(*T)) {
	wg := &sync.WaitGroup{}
	barrier := make(chan struct{}, 1)
	first := newT(wg, barrier)
	current := first
	for _, test := range topTests {
		t := newT(wg, barrier)
		current.next = t
		go runTest(current, test)
		current = t
	}
	first.canRun <- struct{}{}
	<-current.canRun
	close(barrier)
	wg.Wait()
}
