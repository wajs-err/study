//go:build !solution

package hotelbusiness

import "sort"

type Guest struct {
	CheckInDate  int
	CheckOutDate int
}

type Load struct {
	StartDate  int
	GuestCount int
}

func ComputeLoad(guests []Guest) []Load {
	type Date struct {
		Date  int
		Value int
	}

	dates := make([]Date, 0, 2*len(guests))
	for _, g := range guests {
		dates = append(dates, Date{g.CheckInDate, +1})
		dates = append(dates, Date{g.CheckOutDate, -1})
	}

	sort.Slice(dates, func(i, j int) bool {
		return dates[i].Date < dates[j].Date
	})

	currentDate := -1
	currentValue := 0
	prevValue := 0
	result := make([]Load, 0, 2)
	for _, date := range dates {
		if prevValue != currentValue && currentDate != date.Date {
			result = append(result, Load{currentDate, currentValue})
		}
		if currentDate != date.Date {
			currentDate = date.Date
			prevValue = currentValue
		}
		currentValue += date.Value
	}
	if len(dates) > 0 {
		result = append(result, Load{currentDate, currentValue})
	}

	return result
}
