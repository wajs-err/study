//go:build !solution

package speller

import (
	"strings"
)

const (
	delimeter = " "
	minus     = "minus"

	zero      = "zero"
	one       = "one"
	two       = "two"
	three     = "three"
	four      = "four"
	five      = "five"
	six       = "six"
	seven     = "seven"
	eight     = "eight"
	nine      = "nine"
	ten       = "ten"
	eleven    = "eleven"
	twelve    = "twelve"
	thirteen  = "thirteen"
	fourteen  = "fourteen"
	fifteen   = "fifteen"
	sixteen   = "sixteen"
	seventeen = "seventeen"
	eighteen  = "eighteen"
	nineteen  = "nineteen"
	twenty    = "twenty"
	thirty    = "thirty"
	forty     = "forty"
	fifty     = "fifty"
	sixty     = "sixty"
	seventy   = "seventy"
	eighty    = "eighty"
	ninety    = "ninety"
	hundred   = "hundred"
	thousand  = "thousand"
	million   = "million"
	billion   = "billion"
)

func getOnes(n int64) string {
	n = n % 10

	switch n {
	case 0:
		return ""
	case 1:
		return one
	case 2:
		return two
	case 3:
		return three
	case 4:
		return four
	case 5:
		return five
	case 6:
		return six
	case 7:
		return seven
	case 8:
		return eight
	case 9:
		return nine
	default:
		return ""
	}
}

func getTens(n int64) string {
	n = (n % 100) / 10

	switch n {
	case 0:
		return ""
	case 1:
		return ""
	case 2:
		return twenty
	case 3:
		return thirty
	case 4:
		return forty
	case 5:
		return fifty
	case 6:
		return sixty
	case 7:
		return seventy
	case 8:
		return eighty
	case 9:
		return ninety
	default:
		return ""
	}
}

func getTensOnes(n int64) string {
	n = n % 100

	switch n {
	case 0:
		return ""
	case 10:
		return ten
	case 11:
		return eleven
	case 12:
		return twelve
	case 13:
		return thirteen
	case 14:
		return fourteen
	case 15:
		return fifteen
	case 16:
		return sixteen
	case 17:
		return seventeen
	case 18:
		return eighteen
	case 19:
		return nineteen
	default:
		ones := getOnes(n)
		tens := getTens(n)
		if tens != "" && ones != "" {
			return tens + "-" + ones
		}
		if ones != "" {
			return ones
		}
		return tens
	}
}

func getHundredsTensOnes(n int64) string {
	n = n % 1000
	var b strings.Builder

	hundreds := getOnes(n / 100)
	if hundreds != "" {
		b.WriteString(hundreds)
		b.WriteString(delimeter)
		b.WriteString(hundred)
	}

	tensAndOnes := getTensOnes(n)
	if tensAndOnes != "" {
		if hundreds != "" {
			b.WriteString(delimeter)
		}
		b.WriteString(tensAndOnes)
	}

	return b.String()
}

func Spell(n int64) string {
	if n == 0 {
		return zero
	}

	var b strings.Builder
	if n < 0 {
		b.WriteString(minus)
		b.WriteString(delimeter)
		n = -n
	}

	billions := getHundredsTensOnes(n / 1_000_000_000)
	millions := getHundredsTensOnes(n / 1_000_000)
	thousands := getHundredsTensOnes(n / 1_000)
	ones := getHundredsTensOnes(n)

	if billions != "" {
		b.WriteString(billions)
		b.WriteString(delimeter)
		b.WriteString(billion)
	}
	if millions != "" {
		if billions != "" {
			b.WriteString(delimeter)
		}
		b.WriteString(millions)
		b.WriteString(delimeter)
		b.WriteString(million)
	}
	if thousands != "" {
		if billions != "" || millions != "" {
			b.WriteString(delimeter)
		}
		b.WriteString(thousands)
		b.WriteString(delimeter)
		b.WriteString(thousand)
	}
	if ones != "" {
		if billions != "" || millions != "" || thousands != "" {
			b.WriteString(delimeter)
		}
		b.WriteString(ones)
	}

	return b.String()
}
