//go:build !solution

package reversemap

import "reflect"

func ReverseMap(forward interface{}) interface{} {
    kt, vt := reflect.TypeOf(forward).Key(), reflect.TypeOf(forward).Elem()
    backward := reflect.MakeMap(reflect.MapOf(vt, kt))
    iter := reflect.ValueOf(forward).MapRange()
    for iter.Next() {
        backward.SetMapIndex(iter.Value(), iter.Key())
    }
    return backward.Interface()
}
