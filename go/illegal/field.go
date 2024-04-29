//go:build !solution

package illegal

import (
	"reflect"
	"unsafe"
)

func SetPrivateField(obj interface{}, name string, value interface{}) {
    f, ok := reflect.TypeOf(obj).Elem().FieldByName(name)
    if !ok {
        panic("No such field")
    }

    if f.Type != reflect.TypeOf(value) {
        panic("Type doesn't match")
    }
    
    ptr := unsafe.Pointer(reflect.ValueOf(obj).Elem().UnsafeAddr() + f.Offset)
    reflect.NewAt(f.Type, ptr).Elem().Set(reflect.ValueOf(value))
}
