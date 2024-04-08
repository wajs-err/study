//go:build !solution

package dupcall

import (
	"context"
	"sync"
)

type Call struct {
	mu     sync.Mutex
	res    interface{}
	err    error
	ch     chan struct{}
	cancel context.CancelFunc
	count  int
}

func (o *Call) Do(
	ctx context.Context,
	cb func(context.Context) (interface{}, error),
) (result interface{}, err error) {
	defer func() {
		o.mu.Lock()
		defer o.mu.Unlock()
		o.count -= 1
		if o.count == 0 {
			o.cancel()
		}
	}()

	run := false
	o.mu.Lock()
	o.count += 1
	if o.count == 1 {
		run = true
	}
	o.mu.Unlock()

	if run {
		o.ch = make(chan struct{})
		var c context.Context
		c, o.cancel = context.WithCancel(ctx)
		go func(ctx context.Context) {
			o.res, o.err = cb(ctx)
			close(o.ch)
		}(c)
	}

	select {
	case <-o.ch:
		return o.res, o.err
	case <-ctx.Done():
		return nil, ctx.Err()
	}
}
