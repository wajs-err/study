//go:build !solution

package cond

// A Locker represents an object that can be locked and unlocked.
type Locker interface {
	Lock()
	Unlock()
}

// Cond implements a condition variable, a rendezvous point
// for goroutines waiting for or announcing the occurrence
// of an event.
//
// Each Cond has an associated Locker L (often a *sync.Mutex or *sync.RWMutex),
// which must be held when changing the condition and
// when calling the Wait method.
type Cond struct {
	L Locker
	q []int
	// s <- count of goroutines that should be awaken
	s chan int
}

// New returns a new Cond with Locker l.
func New(l Locker) *Cond {
	var c Cond
	c.L = l
	c.q = make([]int, 0)
	c.s = make(chan int, 1)
	return &c
}

// Wait atomically unlocks c.L and suspends execution
// of the calling goroutine. After later resuming execution,
// Wait locks c.L before returning. Unlike in other systems,
// Wait cannot return unless awoken by Broadcast or Signal.
//
// Because c.L is not locked when Wait first resumes, the caller
// typically cannot assume that the condition is true when
// Wait returns. Instead, the caller should Wait in a loop:
//
//	c.L.Lock()
//	for !condition() {
//	    c.Wait()
//	}
//	... make use of condition ...
//	c.L.Unlock()
func (c *Cond) Wait() {
	n := 0
	if len(c.q) != 0 {
		n = c.q[len(c.q)-1] + 1
	}
	c.q = append(c.q, n)
	c.L.Unlock()
	for {
		s := <-c.s
		if s > 0 && c.q[0] == n {
			c.L.Lock()
			c.q = c.q[1:]
			c.L.Unlock()
			if s != 1 {
				c.s <- s - 1
			}
			break
		}
		c.s <- s
	}
	c.L.Lock()
}

// Signal wakes one goroutine waiting on c, if there is any.
//
// It is allowed but not required for the caller to hold c.L
// during the call.
func (c *Cond) Signal() {
	c.s <- 1
}

// Broadcast wakes all goroutines waiting on c.
//
// It is allowed but not required for the caller to hold c.L
// during the call.
func (c *Cond) Broadcast() {
	c.s <- len(c.q)
}
