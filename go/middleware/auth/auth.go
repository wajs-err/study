//go:build !solution

package auth

import (
	"context"
	"errors"
	"net/http"
	"strings"
)

type User struct {
	Name  string
	Email string
}

type UserKey struct{}

func ContextUser(ctx context.Context) (*User, bool) {
	val := ctx.Value(UserKey{})
	if val != nil {
		return val.(*User), true
	}
	return nil, false
}

var ErrInvalidToken = errors.New("invalid token")

type TokenChecker interface {
	CheckToken(ctx context.Context, token string) (*User, error)
}

func CheckAuth(checker TokenChecker) func(next http.Handler) http.Handler {
	return func(next http.Handler) http.Handler {
		fn := func(w http.ResponseWriter, r *http.Request) {
			auth := r.Header.Get("Authorization")
			split := strings.Split(auth, " ")
			if len(split) != 2 || split[0] != "Bearer" {
				http.Error(w, "Not authorized", http.StatusUnauthorized)
				return
			}
			token := split[1]
			user, err := checker.CheckToken(r.Context(), token)
			if err != nil {
				http.Error(w, "Internal server error", http.StatusUnauthorized)
				return
			}
			r = r.WithContext(context.WithValue(r.Context(), UserKey{}, user))
			next.ServeHTTP(w, r)
		}
		return http.HandlerFunc(fn)
	}
}
