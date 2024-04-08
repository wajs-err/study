//go:build !solution

package testequal

func isSameType[U any](e U, actual interface{}) bool {
	switch actual.(type) {
	case U:
		return true
	default:
		return false
	}
}

func slicesEqual[U comparable](e, a []U) bool {
	if e == nil && a != nil || e != nil && a == nil {
		return false
	}
	if len(e) != len(a) {
		return false
	}
	for i := 0; i < len(e); i++ {
		if e[i] != a[i] {
			return false
		}
	}
	return true
}

func mapsEqual[K, V comparable](e, a map[K]V) bool {
	if e == nil && a != nil || e != nil && a == nil {
		return false
	}
	for k, v := range e {
		if a[k] != v {
			return false
		}
	}
	for k, v := range a {
		if e[k] != v {
			return false
		}
	}
	return true
}

func setErrorMessage(t T, msgAndArgs ...interface{}) {
	t.Helper()
	if len(msgAndArgs) >= 1 {
		t.Errorf(msgAndArgs[0].(string), msgAndArgs[1:]...)
	} else {
		t.Errorf("")
	}
}

func testEqualHelper(failOnNotEqual bool, t T, expected, actual interface{}, msgAndArgs ...interface{}) bool {
	t.Helper()
	switch e := expected.(type) {
	case int:
		if isSameType(e, actual) && e == actual.(int) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case int8:
		if isSameType(e, actual) && e == actual.(int8) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case int16:
		if isSameType(e, actual) && e == actual.(int16) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case int32:
		if isSameType(e, actual) && e == actual.(int32) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case int64:
		if isSameType(e, actual) && e == actual.(int64) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case uint:
		if isSameType(e, actual) && e == actual.(uint) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case uint8:
		if isSameType(e, actual) && e == actual.(uint8) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case uint16:
		if isSameType(e, actual) && e == actual.(uint16) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case uint32:
		if isSameType(e, actual) && e == actual.(uint32) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case uint64:
		if isSameType(e, actual) && e == actual.(uint64) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case string:
		if isSameType(e, actual) && e == actual.(string) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case []int:
		if isSameType(e, actual) && slicesEqual(e, actual.([]int)) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case []byte:
		if isSameType(e, actual) && slicesEqual(e, actual.([]byte)) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case map[string]string:
		if isSameType(e, actual) && mapsEqual(e, actual.(map[string]string)) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	case map[int]int:
		if isSameType(e, actual) && mapsEqual(e, actual.(map[int]int)) {
			if !failOnNotEqual {
				setErrorMessage(t, msgAndArgs...)
			}
			return true
		}
		if failOnNotEqual {
			setErrorMessage(t, msgAndArgs...)
		}
		return false
	default:
		panic("unsupported type")
	}
}

// AssertEqual checks that expected and actual are equal.
//
// Marks caller function as having failed but continues execution.
//
// Returns true iff arguments are equal.
func AssertEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) bool {
	defer func() bool {
		if r := recover(); r != nil {
			return false
		}
		return false
	}()
	t.Helper()
	return testEqualHelper(true, t, expected, actual, msgAndArgs...)
}

// AssertNotEqual checks that expected and actual are not equal.
//
// Marks caller function as having failed but continues execution.
//
// Returns true iff arguments are not equal.
func AssertNotEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) bool {
	defer func() bool {
		if r := recover(); r != nil {
			return false
		}
		return false
	}()
	t.Helper()
	return !testEqualHelper(false, t, expected, actual, msgAndArgs...)
}

// RequireEqual does the same as AssertEqual but fails caller test immediately.
func RequireEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) {
	defer func() {
		_ = recover()
	}()
	t.Helper()
	if !testEqualHelper(true, t, expected, actual, msgAndArgs...) {
		t.FailNow()
	}
}

// RequireNotEqual does the same as AssertNotEqual but fails caller test immediately.
func RequireNotEqual(t T, expected, actual interface{}, msgAndArgs ...interface{}) {
	defer func() {
		_ = recover()
	}()
	t.Helper()
	if testEqualHelper(false, t, expected, actual, msgAndArgs...) {
		t.FailNow()
	}
}
