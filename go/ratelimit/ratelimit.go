//go:build !solution

package ratelimit

import (
	"context"
	"errors"
	"time"
)

type timeout struct {
	ctx    context.Context
	cancel context.CancelFunc
}

// Limiter is precise rate limiter with context support.
type Limiter struct {
	maxCount int
	interval time.Duration
	timeouts []*time.Timer
	stopped  bool
}

var ErrStopped = errors.New("limiter stopped")

// NewLimiter returns limiter that throttles rate of successful Acquire() calls
// to maxSize events at any given interval.
func NewLimiter(maxCount int, interval time.Duration) *Limiter {
	timeouts := make([]*time.Timer, maxCount)
	for i := range timeouts {
		timeouts[i] = time.NewTimer(0)
	}
	return &Limiter{maxCount: maxCount, interval: interval, timeouts: timeouts, stopped: false}
}

func (l *Limiter) Acquire(ctx context.Context) error {
	if l.stopped {
		return ErrStopped
	}

	for i := 0; ; i = (i + 1) % l.maxCount {
		select {
		case <-ctx.Done():
			return ctx.Err()
		default:
			break
		}

		select {
		case <-ctx.Done():
			return ctx.Err()
		case <-l.timeouts[i].C:
			l.timeouts[i] = time.NewTimer(l.interval)
			return nil
		default:
			break
		}
	}

}

func (l *Limiter) Stop() {
	l.stopped = true
}
