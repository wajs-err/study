//go:build !solution

package genericsum

import (
	"cmp"
	"math/cmplx"
	"sync"
)

func Min[T cmp.Ordered](a, b T) T {
	if a < b {
		return a
	}
	return b
}

func SortSlice[T cmp.Ordered](a []T) {
	if len(a) <= 1 {
		return
	}
	if len(a) == 2 {
		if a[0] <= a[1] {
			return
		}
		a[0], a[1] = a[1], a[0]
		return
	}

	pivot := a[len(a)-1]
	i, j := 0, len(a)-2
	for i < j {
		if a[i] <= pivot {
			i++
			continue
		}
		if a[j] >= pivot {
			j--
			continue
		}
		a[i], a[j] = a[j], a[i]
	}
	if i != len(a)-2 {
		a[i], a[len(a)-1] = pivot, a[i]
	}
	SortSlice(a[:i+1])
	SortSlice(a[i+1:])
}

func MapsEqual[K, V comparable](a, b map[K]V) bool {
	for k, v := range a {
		if u, ok := b[k]; !ok || u != v {
			return false
		}
	}
	for k, v := range b {
		if u, ok := a[k]; !ok || u != v {
			return false
		}
	}
	return true
}

func SliceContains[T comparable](s []T, v T) bool {
	for _, u := range s {
		if u == v {
			return true
		}
	}
	return false
}

func MergeChans[T any](chs ...<-chan T) <-chan T {
	var wg sync.WaitGroup
	wg.Add(len(chs))
	res := make(chan T)
	for _, ch := range chs {
		c := ch
		go func() {
			for v := range c {
				res <- v
			}
			wg.Done()
		}()
	}
	go func() {
		wg.Wait()
		close(res)
	}()
	return res
}

type complexLike interface {
	int | complex64 | complex128
}

func IsHermitianMatrix[T complexLike](m [][]T) bool {
	for i, r := range m {
		for j := range r {
			switch v := any(m[i][j]).(type) {
			case int:
				if m[i][j] != m[j][i] {
					return false
				}
			case complex64:
				if cmplx.Conj(complex128(v)) != complex128(any(m[j][i]).(complex64)) {
					return false
				}
			case complex128:
				if cmplx.Conj(v) != any(m[j][i]).(complex128) {
					return false
				}
			}
		}
	}
	return true
}
