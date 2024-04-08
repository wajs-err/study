//go:build !solution

package requestlog

import (
	"crypto/rand"
	"fmt"
	"net/http"

	"github.com/felixge/httpsnoop"
	"go.uber.org/zap"
)

func Log(l *zap.Logger) func(next http.Handler) http.Handler {
	return func(next http.Handler) http.Handler {
		fn := func(w http.ResponseWriter, r *http.Request) {
			b := make([]byte, 10)
			_, _ = rand.Read(b)
			requestID := fmt.Sprintf("%x", b)

			l.Info("request started",
				zap.String("request_id", requestID),
				zap.String("path", r.URL.Path),
				zap.String("method", r.Method),
			)
			defer func() {
				if rec := recover(); rec != nil {
					l.Info("request panicked",
						zap.String("request_id", requestID),
						zap.String("path", r.URL.Path),
						zap.String("method", r.Method),
					)
					panic(rec)
				}
			}()

			m := httpsnoop.CaptureMetrics(next, w, r)

			l.Info("request finished",
				zap.String("request_id", requestID),
				zap.String("path", r.URL.Path),
				zap.String("method", r.Method),
				zap.Int("status_code", m.Code),
				zap.Duration("duration", m.Duration),
			)
		}
		return http.HandlerFunc(fn)
	}
}
