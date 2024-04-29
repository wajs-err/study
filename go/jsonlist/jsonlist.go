//go:build !solution

package jsonlist

import (
	"encoding/json"
	"io"
	"reflect"
)

func Marshal(w io.Writer, slice interface{}) (err error) {
	defer func() {
		if r := recover(); r != nil {
			err = &json.UnsupportedTypeError{Type: reflect.TypeOf(slice)}
		}
	}()

	e := json.NewEncoder(w)
	for i := 0; i < reflect.ValueOf(slice).Len(); i++ {
		if err := e.Encode(reflect.ValueOf(slice).Index(i).Interface()); err != nil {
			return err
		}
	}
	return nil
}

func Unmarshal(r io.Reader, slice interface{}) (err error) {
	defer func() {
		if r := recover(); r != nil {
			err = &json.UnsupportedTypeError{Type: reflect.TypeOf(slice)}
		}
	}()

	d := json.NewDecoder(r)
	typ := reflect.TypeOf(slice).Elem().Elem()
	for {
		v := reflect.New(typ)
		err = d.Decode(v.Interface())
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}
		tmpSlice := reflect.Append(reflect.ValueOf(slice).Elem(), v.Elem())
		reflect.ValueOf(slice).Elem().Set(tmpSlice)
	}
	return nil
}
