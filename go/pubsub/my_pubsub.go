//go:build !solution

package pubsub

import (
	"context"
	"errors"
	"sync"
)

var _ Subscription = (*MySubscription)(nil)

type MySubscription struct {
	cb    MsgHandler
	m     *sync.Mutex
	queue []interface{}
	done  bool
}

func (s *MySubscription) Unsubscribe() {
	s.done = true
}

func (s *MySubscription) handleMsg() {
	for {
		s.m.Lock()
		if len(s.queue) == 0 {
			s.m.Unlock()
			break
		}
		msg := s.queue[0]
		s.m.Unlock()

		s.cb(msg)

		s.m.Lock()
		s.queue = s.queue[1:]
		s.m.Unlock()
	}
}

var _ PubSub = (*MyPubSub)(nil)

type topic struct {
	m    *sync.Mutex
	subs []*MySubscription
}

type MyPubSub struct {
	t      map[string]topic
	closed bool
}

var pubSubClosedError = errors.New("pubsub was closed")

func NewPubSub() PubSub {
	return &MyPubSub{t: map[string]topic{}, closed: false}
}

func (p *MyPubSub) Subscribe(subj string, cb MsgHandler) (Subscription, error) {
	if p.closed {
		return nil, pubSubClosedError
	}

	if _, ok := p.t[subj]; !ok {
		p.t[subj] = topic{m: &sync.Mutex{}, subs: make([]*MySubscription, 0)}
	}

	t := p.t[subj]
	t.m.Lock()
	defer t.m.Unlock()
	t.subs = append(t.subs, &MySubscription{cb: cb, queue: make([]interface{}, 0), m: &sync.Mutex{}, done: false})
	p.t[subj] = t
	return t.subs[len(t.subs)-1], nil
}

func (p *MyPubSub) Publish(subj string, msg interface{}) error {
	if p.closed {
		return pubSubClosedError
	}

	if _, ok := p.t[subj]; !ok {
		p.t[subj] = topic{m: &sync.Mutex{}, subs: make([]*MySubscription, 0)}
	}

	t := p.t[subj]
	t.m.Lock()
	defer t.m.Unlock()
	for i := range t.subs {
		if !p.t[subj].subs[i].done {
			p.t[subj].subs[i].m.Lock()
			p.t[subj].subs[i].queue = append(p.t[subj].subs[i].queue, msg)
			if len(p.t[subj].subs[i].queue) == 1 {
				go p.t[subj].subs[i].handleMsg()
			}
			p.t[subj].subs[i].m.Unlock()
		}
	}
	return nil
}

func (p *MyPubSub) Close(ctx context.Context) error {
	select {
	case <-ctx.Done():
		return errors.New("context timed out")
	default:
		p.closed = true
		return nil
	}
}
