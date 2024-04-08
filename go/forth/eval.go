//go:build !solution

package main

import (
	"errors"
	"strconv"
	"strings"
)

type Op int

const (
	Plus Op = iota
	Minus
	Multiply
	Divide
	Dup
	Over
	Drop
	Swap
)

type Evaluator struct {
	stack []int
	words map[string][]interface{}
}

func NewEvaluator() *Evaluator {
	var e Evaluator
	e.stack = make([]int, 0)
	e.words = map[string][]interface{}{
		"+":    {Plus},
		"-":    {Minus},
		"*":    {Multiply},
		"/":    {Divide},
		"dup":  {Dup},
		"over": {Over},
		"drop": {Drop},
		"swap": {Swap},
	}
	return &e
}

func (e *Evaluator) processPlus() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `+`")
	}
	e.stack[len(e.stack)-2] += e.stack[len(e.stack)-1]
	e.stack = e.stack[:len(e.stack)-1]
	return nil
}

func (e *Evaluator) processMinus() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `-`")
	}
	e.stack[len(e.stack)-2] -= e.stack[len(e.stack)-1]
	e.stack = e.stack[:len(e.stack)-1]
	return nil
}

func (e *Evaluator) processMultiply() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `*`")
	}
	e.stack[len(e.stack)-2] *= e.stack[len(e.stack)-1]
	e.stack = e.stack[:len(e.stack)-1]
	return nil
}

func (e *Evaluator) processDivide() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `/`")
	}
	if e.stack[len(e.stack)-1] == 0 {
		return errors.New("division by zero")
	}
	e.stack[len(e.stack)-2] /= e.stack[len(e.stack)-1]
	e.stack = e.stack[:len(e.stack)-1]
	return nil
}

func (e *Evaluator) processDup() error {
	if len(e.stack) < 1 {
		return errors.New("not enough arguments for `dup`")
	}
	e.stack = append(e.stack, e.stack[len(e.stack)-1])
	return nil
}

func (e *Evaluator) processOver() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `over`")
	}
	e.stack = append(e.stack, e.stack[len(e.stack)-2])
	return nil
}

func (e *Evaluator) processDrop() error {
	if len(e.stack) == 0 {
		return errors.New("stack is empty before `drop`")
	}
	e.stack = e.stack[:len(e.stack)-1]
	return nil
}

func (e *Evaluator) processNumber(num string) error {
	if n, err := strconv.Atoi(num); err == nil {
		e.stack = append(e.stack, n)
		return nil
	}
	return errors.New("unexpected keyword")
}

func (e *Evaluator) processSwap() error {
	if len(e.stack) < 2 {
		return errors.New("not enough arguments for `swap`")
	}
	e.stack[len(e.stack)-2], e.stack[len(e.stack)-1] =
		e.stack[len(e.stack)-1], e.stack[len(e.stack)-2]
	return nil
}

func (e *Evaluator) processOp(op Op) error {
	switch op {
	case Plus:
		return e.processPlus()
	case Minus:
		return e.processMinus()
	case Multiply:
		return e.processMultiply()
	case Divide:
		return e.processDivide()
	case Dup:
		return e.processDup()
	case Over:
		return e.processOver()
	case Drop:
		return e.processDrop()
	case Swap:
		return e.processSwap()
	default:
		return errors.New("unexpected command")
	}
}

func (e *Evaluator) processCmd(cmd string) error {
	if _, ok := e.words[cmd]; !ok {
		return e.processNumber(cmd)
	}
	for _, op := range e.words[cmd] {
		switch op := op.(type) {
		case Op:
			if err := e.processOp(op); err != nil {
				return err
			}
		case string:
			if err := e.processNumber(op); err != nil {
				return err
			}
		default:
			return errors.New("unexpected command")
		}
	}
	return nil
}

func (e *Evaluator) addCmd(name string, cmds []string) {
	words := make([]interface{}, 0)
	for _, cmd := range cmds {
		if ws, ok := e.words[cmd]; ok {
			words = append(words, ws...)
		} else {
			words = append(words, cmd)
		}
	}
	e.words[name] = words
}

func (e *Evaluator) Process(row string) ([]int, error) {
	cmds := strings.Split(strings.ToLower(row), " ")
	defBegin := -1
	for i, cmd := range cmds {
		if cmd != ";" && defBegin != -1 {
			continue
		}
		if cmd == ";" {
			if i-defBegin < 3 {
				return e.stack, errors.New("not enough arguments to create new command")
			}
			if _, err := strconv.Atoi(cmds[defBegin+1]); err == nil {
				return e.stack, errors.New("can not redefine number")
			}
			e.addCmd(cmds[defBegin+1], cmds[defBegin+2:i])
			defBegin = -1
			continue
		}
		if cmd == ":" {
			defBegin = i
			continue
		}

		if err := e.processCmd(cmd); err != nil {
			return e.stack, err
		}
	}

	return e.stack, nil
}
