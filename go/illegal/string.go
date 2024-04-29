//go:build !solution

package illegal

import (
	"unsafe"
)

func StringFromBytes(b []byte) string {
    return unsafe.String(unsafe.SliceData(b), len(b))
}
