//go:build !solution

package spacecollapse

import (
	"strings"
	"unicode"
	"unicode/utf8"
)

func CollapseSpaces(input string) string {
	const replacement = '\ufffd'
	const space = ' '
	var b strings.Builder
	b.Grow(len(input))
	prevSpace := false
	for i, w := 0, 0; i < len(input); i += w {
		r, width := utf8.DecodeRuneInString(input[i:])
		w = width
		if w == 1 && !utf8.RuneStart(byte(r)) {
			b.WriteRune(replacement)
		}
		if !unicode.IsSpace(r) {
			b.WriteRune(r)
			prevSpace = false
		} else if !prevSpace {
			b.WriteRune(space)
			prevSpace = true
		}
	}

	return b.String()
}
