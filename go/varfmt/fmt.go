//go:build !solution

package varfmt

import (
	"fmt"
	"strconv"
	"strings"
)

func Sprintf(format string, args ...interface{}) string {
	var b strings.Builder
	b.Grow(len(format))

	currReplacement := 0
	shouldReplace := false
	replaceBy := 0
	for i := 0; i < len(format); i++ {
		if format[i] == '{' {
			shouldReplace = true
			continue
		}

		if format[i] == '}' {
			if replaceBy == -1 {
				replaceBy = currReplacement
			}
			b.WriteString(fmt.Sprint(args[replaceBy]))
			replaceBy = -1
			currReplacement++
			shouldReplace = false
			continue
		}

		if shouldReplace {
			end := i
			for format[end] != '}' {
				end++
			}
			replaceBy, _ = strconv.Atoi(format[i:end])
			i = end - 1
			continue
		}

		b.WriteByte(format[i])
	}

	return b.String()
}
