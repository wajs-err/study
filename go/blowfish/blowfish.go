//go:build !solution

package blowfish

// #cgo pkg-config: libcrypto
// #cgo CFLAGS: -Wno-deprecated-declarations
// #include <openssl/blowfish.h>
import "C"
import "unsafe"

type Blowfish struct {
    key C.BF_KEY
}

// it is probably unsafe, because key may be deallocated/moved
func New(key []byte) *Blowfish {
    b := &Blowfish{}
    _, _ = C.BF_set_key(
        (*C.BF_KEY)(unsafe.Pointer(&b.key)),
        C.int(len(key)),
        (*C.uchar)(unsafe.Pointer(unsafe.SliceData(key))))
    return b 
}

func (b *Blowfish) BlockSize() int {
    return 64
}

// it is probably unsafe, because key may be deallocated/moved
func (b *Blowfish) Encrypt(out, in []byte) {
    _, _ = C.BF_ecb_encrypt(
        (*C.uchar)(unsafe.Pointer(unsafe.SliceData(in))),
        (*C.uchar)(unsafe.Pointer(unsafe.SliceData(out))),
        (*C.BF_KEY)(unsafe.Pointer(&b.key)),
        C.BF_ENCRYPT)
}

// it is probably unsafe, because key may be deallocated/moved
func (b *Blowfish) Decrypt(out, in []byte) {
    _, _ = C.BF_ecb_encrypt(
        (*C.uchar)(unsafe.Pointer(unsafe.SliceData(in))),
        (*C.uchar)(unsafe.Pointer(unsafe.SliceData(out))),
        (*C.BF_KEY)(unsafe.Pointer(&b.key)),
        C.BF_DECRYPT)
}
