//go:build !solution

package main

import (
	"bytes"
	"flag"
	"io"
	"log"
	"net/http"
	"net/url"
	"os"
	"regexp"
	"strings"

	"gopkg.in/yaml.v2"
)

type EndpointConfig struct {
	Endpoint               string   `yaml:"endpoint"`
	ForbiddenUserAgents    []string `yaml:"forbidden_user_agents"`
	ForbiddenHeaders       []string `yaml:"forbidden_headers"`
	RequiredHeaders        []string `yaml:"required_headers"`
	MaxRequestLengthBytes  *int64   `yaml:"max_request_length_bytes"`
	MaxResponseLengthBytes *int64   `yaml:"max_response_length_bytes"`
	ForbiddenResponseCodes []int    `yaml:"forbidden_response_codes"`
	ForbiddenRequestRe     []string `yaml:"forbidden_request_re"`
	ForbiddenResponseRe    []string `yaml:"forbidden_response_re"`
}

type configInternal struct {
	Rules []EndpointConfig `yaml:"rules"`
}

type Config map[string]EndpointConfig

func ParseConfig(path string) Config {
	yamlConfig, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	configArray := configInternal{}
	err = yaml.Unmarshal(yamlConfig, &configArray)
	if err != nil {
		panic(err)
	}

	config := Config{}
	for _, v := range configArray.Rules {
		config[v.Endpoint] = v
	}
	return config
}

func ParseArgs() (*url.URL, string, string) {
	serviceAddr := flag.String("service-addr", "http://localhost:8080", "address of service")
	conf := flag.String("conf", "./firewall/configs/example.yaml", "path to config")
	addr := flag.String("addr", "localhost:8000", "address for firewall")
	flag.Parse()

	url, err := url.Parse(*serviceAddr)
	if err != nil {
		panic(err)
	}

	return url, *conf, *addr
}

var _ http.RoundTripper = (*Firewall)(nil)

type Firewall struct {
	ServiceURL *url.URL
	Config     Config
}

func DefaultForbiddenReponse() *http.Response {
	return &http.Response{
		StatusCode: http.StatusForbidden,
		Body:       io.NopCloser(bytes.NewBufferString("Forbidden")),
	}
}

func (f *Firewall) CheckRequestLength(req *http.Request) *http.Response {
	m := f.Config[req.URL.Path].MaxRequestLengthBytes
	if m != nil && req.ContentLength > *m {
		return DefaultForbiddenReponse()
	}
	return nil
}

func (f *Firewall) CheckForbiddenRequestUserAgents(req *http.Request) *http.Response {
	agents := req.Header.Values("User-Agent")
	for _, re := range f.Config[req.URL.Path].ForbiddenUserAgents {
		for _, a := range agents {
			match, err := regexp.Match(re, []byte(a))
			if err != nil {
				panic(err)
			}
			if match {
				return DefaultForbiddenReponse()
			}
		}
	}
	return nil
}

func (f *Firewall) CheckRequestForbiddenHeaders(req *http.Request) *http.Response {
	w := &strings.Builder{}
	req.Header.Write(w)
	for _, re := range f.Config[req.URL.Path].ForbiddenHeaders {
		match, err := regexp.Match(re, []byte(w.String()))
		if err != nil {
			panic(err)
		}
		if match {
			return DefaultForbiddenReponse()
		}
	}
	return nil
}

func (f *Firewall) CheckRequestRequiredHeaders(req *http.Request) *http.Response {
	w := &strings.Builder{}
	req.Header.Write(w)
	for _, re := range f.Config[req.URL.Path].RequiredHeaders {
		match, err := regexp.Match(re, []byte(w.String()))
		if err != nil {
			panic(err)
		}
		if !match {
			return DefaultForbiddenReponse()
		}
	}
	return nil
}

func (f *Firewall) CheckRequestBody(req *http.Request) *http.Response {
	b, err := io.ReadAll(req.Body)
	if err != nil {
		panic(err)
	}
	req.Body = io.NopCloser(bytes.NewBuffer(b))
	for _, re := range f.Config[req.URL.Path].ForbiddenRequestRe {
		match, err := regexp.Match(re, b)
		if err != nil {
			panic(err)
		}
		if match {
			return DefaultForbiddenReponse()
		}
	}

	return nil
}

func (f *Firewall) CheckResponseLength(resp *http.Response, url string) *http.Response {
	m := f.Config[url].MaxResponseLengthBytes
	if m != nil && resp.ContentLength > *m {
		return DefaultForbiddenReponse()
	}
	return nil
}

func (f *Firewall) CheckForbiddenResponseCodes(resp *http.Response, url string) *http.Response {
	for _, c := range f.Config[url].ForbiddenResponseCodes {
		if resp.StatusCode == c {
			return DefaultForbiddenReponse()
		}
	}
	return nil
}

func (f *Firewall) CheckResponseBody(resp *http.Response, url string) *http.Response {
	b, err := io.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	resp.Body = io.NopCloser(bytes.NewBuffer(b))
	for _, re := range f.Config[url].ForbiddenResponseRe {
		match, err := regexp.Match(re, b)
		if err != nil {
			panic(err)
		}
		if match {
			return DefaultForbiddenReponse()
		}
	}

	return nil
}

func (f *Firewall) RoundTrip(req *http.Request) (*http.Response, error) {
	if _, ok := f.Config[req.URL.Path]; ok {
		if resp := f.CheckRequestLength(req); resp != nil {
			return resp, nil
		}
		if resp := f.CheckRequestForbiddenHeaders(req); resp != nil {
			return resp, nil
		}
		if resp := f.CheckRequestRequiredHeaders(req); resp != nil {
			return resp, nil
		}
		if resp := f.CheckRequestBody(req); resp != nil {
			return resp, nil
		}
		if resp := f.CheckForbiddenRequestUserAgents(req); resp != nil {
			return resp, nil
		}
	}

	url := strings.Clone(req.URL.Path)
	req.URL = f.ServiceURL
	resp, err := http.DefaultTransport.RoundTrip(req)
	if err != nil {
		return nil, err
	}

	if _, ok := f.Config[url]; ok {
		if updatedResp := f.CheckResponseLength(resp, url); updatedResp != nil {
			return updatedResp, nil
		}
		if updatedResp := f.CheckForbiddenResponseCodes(resp, url); updatedResp != nil {
			return updatedResp, nil
		}
		if updatedResp := f.CheckResponseBody(resp, url); updatedResp != nil {
			return updatedResp, nil
		}
	}

	return resp, nil
}

func main() {
	serviceAddr, conf, addr := ParseArgs()
	config := ParseConfig(conf)
	c := http.Client{Transport: &Firewall{ServiceURL: serviceAddr, Config: config}}

	mux := http.NewServeMux()
	mux.HandleFunc("/", func(w http.ResponseWriter, req *http.Request) {
		req.RequestURI = ""
		resp, err := c.Do(req)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			return
		}

		w.WriteHeader(resp.StatusCode)
		for h, vs := range resp.Header {
			for _, v := range vs {
				w.Header().Add(h, v)
			}
		}
		defer resp.Body.Close()
		io.Copy(w, resp.Body)
	})
	log.Fatal(http.ListenAndServe(addr, mux))
}
