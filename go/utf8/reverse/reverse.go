//go:build !solution

package reverse

import (
	"strings"
	"unicode/utf8"
)

func Reverse(input string) string {
	var b strings.Builder
	b.Grow(len(input))
	for i, w := len(input), 0; i > 0; i -= w {
		r, width := utf8.DecodeLastRuneInString(input[:i])
		b.WriteRune(r)
		w = width
	}
	return b.String()
}
