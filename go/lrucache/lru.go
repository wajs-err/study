//go:build !solution

package lrucache

import "container/list"

type KeyValue struct {
	key   int
	value int
}

type LruCache struct {
	m   map[int]*list.Element
	l   list.List
	cap int
}

func (c *LruCache) Get(key int) (int, bool) {
	if e, ok := c.m[key]; ok {
		c.l.MoveToFront(e)
		value, _ := e.Value.(KeyValue)
		return value.value, true
	}
	return 0, false
}

func (c *LruCache) Set(key, value int) {
	if e, ok := c.m[key]; ok {
		c.l.MoveToFront(e)
		e.Value = KeyValue{key, value}
		return
	}

	if c.l.Len() == c.cap {
		if b := c.l.Back(); b == nil {
			return
		}
		r := c.l.Remove(c.l.Back())
		delete(c.m, r.(KeyValue).key)
	}
	e := c.l.PushFront(KeyValue{key, value})
	c.m[key] = e
}

func (c *LruCache) Range(f func(key, value int) bool) {
	e := c.l.Back()
	for e != nil && f(e.Value.(KeyValue).key, e.Value.(KeyValue).value) {
		e = e.Prev()
	}
}

func (c *LruCache) Clear() {
	c.l.Init()
	c.m = make(map[int]*list.Element)
}

func New(cap int) Cache {
	var c LruCache
	c.m = make(map[int]*list.Element)
	c.l.Init()
	c.cap = cap
	return &c
}
