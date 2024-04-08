//go:build !solution

package treeiter

type node[T any] interface {
	Left() *T
	Right() *T
}

func DoInOrder[T node[T]](n *T, f func(*T)) {
	if n == nil {
		return
	}
	DoInOrder((*n).Left(), f)
	f(n)
	DoInOrder((*n).Right(), f)
}
