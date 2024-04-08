//go:build !solution

package retryupdate

import (
	"errors"

	"github.com/gofrs/uuid"
	"gitlab.com/slon/shad-go/retryupdate/kvapi"
)

func UpdateValue(c kvapi.Client, key string, updateFn func(oldValue *string) (newValue string, err error)) error {
	var oldVersion uuid.UUID
	for {
		var err error
		var getResponse *kvapi.GetResponse
	get:
		for {
			getResponse, err = c.Get(&kvapi.GetRequest{Key: key})
			if err == nil {
				oldVersion = getResponse.Version
				break
			}

			var authErr *kvapi.AuthError
			switch {
			case errors.As(err, &authErr):
				return err
			case errors.Is(err, kvapi.ErrKeyNotFound):
				oldVersion = uuid.UUID{}
				getResponse = nil
				break get
			default:
				continue
			}
		}

		newVersion := uuid.Must(uuid.NewV4())
	set:
		for {
			var newValue string
			if getResponse != nil {
				newValue, err = updateFn(&getResponse.Value)
			} else {
				newValue, err = updateFn(nil)
			}
			if err != nil {
				return err
			}

			_, err = c.Set(&kvapi.SetRequest{
				Key:        key,
				Value:      newValue,
				OldVersion: oldVersion,
				NewVersion: newVersion,
			})
			if err == nil {
				return nil
			}

			var conflictErr *kvapi.ConflictError
			var authErr *kvapi.AuthError
			switch {
			case errors.As(err, &authErr):
				return err
			case errors.As(err, &conflictErr):
				if conflictErr.ExpectedVersion == newVersion {
					return nil
				}
				break set
			case errors.Is(err, kvapi.ErrKeyNotFound):
				oldVersion = uuid.UUID{}
				getResponse = nil
			}
		}
	}
}
