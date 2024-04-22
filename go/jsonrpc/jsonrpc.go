//go:build !solution

package jsonrpc

import (
	"bytes"
	"context"
	"encoding/json"
	"errors"
	"net/http"
	"reflect"
)

type rpcRequest struct {
    Method  string
    Context context.Context
    Request interface{}
}

type rpcResponse struct {
    HasError bool  `json:"has_error"`
    Error    string 
    Response interface{}
}

var _ http.Handler = (*rpcService)(nil)

type rpcService struct {
    service interface{}
}

func (s *rpcService) ServeHTTP(w http.ResponseWriter, req *http.Request) {
    rpcReq := &rpcRequest{}
    _ = json.NewDecoder(req.Body).Decode(rpcReq)
    req.Body.Close()

    reqType := reflect.ValueOf(s.service).MethodByName(rpcReq.Method).Type().In(1).Elem()
    rreq := reflect.New(reqType)
    for n, v := range rpcReq.Request.(map[string]interface{}) {
        f, _ := reqType.FieldByName(n)
        switch rreq.Elem().Field(f.Index[0]).Kind() {
        case reflect.Int:
            i := int(reflect.ValueOf(v).Interface().(float64))
            rreq.Elem().Field(f.Index[0]).Set(reflect.ValueOf(i))
        // other types go here
        }
    }

    rctx := reflect.ValueOf(context.Background())
    rres := reflect.ValueOf(s.service).MethodByName(rpcReq.Method).Call([]reflect.Value{rctx, rreq})

    rpcResp := &rpcResponse{Response: rres[0].Interface(), HasError: false}
    if rres[1].Interface() != nil {
        rpcResp.HasError = true
        rpcResp.Error = rres[1].Interface().(error).Error()
    }

    _ = json.NewEncoder(w).Encode(rpcResp)
}

func MakeHandler(service interface{}) http.Handler {
    return &rpcService{service: service}
}

func Call(ctx context.Context, endpoint string, method string, req, rsp interface{}) error {
    rpcReq, _ := json.Marshal(rpcRequest{Method: method, Context: ctx, Request: req})

    // TODO: http.NewRequestWithContext()
    resp, err := http.Post(endpoint, "application/json", bytes.NewBuffer(rpcReq))
    if err != nil {
        return err
    }

    rpcResp := &rpcResponse{}
    json.NewDecoder(resp.Body).Decode(rpcResp)
    resp.Body.Close()
    if rpcResp.HasError {
        return errors.New(rpcResp.Error)
    }

    respType := reflect.ValueOf(rsp).Type().Elem()
    rresp := reflect.ValueOf(rsp)
    for n, v := range rpcResp.Response.(map[string]interface{}) {
        f, _ := respType.FieldByName(n)
        switch rresp.Elem().Field(f.Index[0]).Kind() {
        case reflect.Int:
            i := int(reflect.ValueOf(v).Interface().(float64))
            rresp.Elem().Field(f.Index[0]).Set(reflect.ValueOf(i))
        // other types go here
        }
    }
    return nil
}
