//go:build !solution

package keylock

import (
	"sync"
)

type chanWrapper struct {
	l chan struct{}
	w chan struct{}
	u chan struct{}
}

type KeyLock struct {
	m  map[string]*chanWrapper
	mu sync.RWMutex
}

func New() *KeyLock {
	return &KeyLock{m: make(map[string]*chanWrapper)}
}

func (l *KeyLock) unlockAllKeys(keys []string) {
	l.mu.RLock()
	defer l.mu.RUnlock()
	for _, key := range keys {
		close(l.m[key].w)
		l.m[key].l <- struct{}{}
		l.m[key].u <- struct{}{}
	}
}

func (l *KeyLock) LockKeys(keys []string, cancel <-chan struct{}) (canceled bool, unlock func()) {
	locked := 0
	for locked != len(keys) {
	tryLockKeys:
		for i, key := range keys {
			if _, ok := l.m[key]; !ok {
				l.mu.Lock()
				l.m[key] = &chanWrapper{l: make(chan struct{}, 1), w: make(chan struct{}), u: make(chan struct{}, 1)}
				locked += 1
				l.mu.Unlock()
				continue
			}

			l.mu.RLock()
			select {
			case <-l.m[key].l:
				locked += 1
				c := l.m[key]
				c.w = make(chan struct{})
				<-l.m[key].u
				l.mu.RUnlock()
			case <-cancel:
				l.unlockAllKeys(keys[:i])
				l.mu.RUnlock()
				return true, func() {}
			default:
				locked = 0
				l.unlockAllKeys(keys[:i])
				l.mu.RUnlock()
				select {
				case <-cancel:
					return true, func() {}
				case <-l.m[key].w:
					break tryLockKeys
				}
			}
		}
	}

	return false, func() {
		l.unlockAllKeys(keys)
	}
}
