//go:build !solution

package ciletters

import (
	_ "embed"
	"strings"
	"text/template"
)

//go:embed letter_template.txt
var tmplString string

func MakeLetter(n *Notification) (string, error) {
	funcMap := template.FuncMap{
		"lines": func(log string) string {
			lines := strings.Split(log, "\n")
			for i := range lines {
				lines[i] = "            " + lines[i] + "\n"
			}
			return strings.Join(lines[max(0, len(lines)-10):], "")
		},
	}

	tmpl, err := template.
		New("letter").
		Funcs(funcMap).
		Parse(tmplString)
	if err != nil {
		panic("can not generate template")
	}

	var b strings.Builder
	err = tmpl.Execute(&b, *n)
	return b.String(), err
}
