//go:build !solution

package main

import (
	"encoding/json"
	"errors"
	"flag"
	"log"
	"net/http"
	"os"
	"sort"
	"strconv"
)

type MedalsInfo struct {
	Gold   int `json:"gold"`
	Silver int `json:"silver"`
	Bronze int `json:"bronze"`
	Total  int `json:"total"`
}

func (mi *MedalsInfo) Add(other MedalsInfo) {
	mi.Gold += other.Gold
	mi.Silver += other.Silver
	mi.Bronze += other.Bronze
	mi.Total += other.Total
}

func (mi *MedalsInfo) Less(other MedalsInfo) bool {
	return mi.Gold < other.Gold ||
		mi.Gold == other.Gold && mi.Silver < other.Silver ||
		mi.Gold == other.Gold && mi.Silver == other.Silver && mi.Bronze < other.Bronze
}

type AthleteData struct {
	Name    string `json:"athlete"`
	Age     int
	Country string
	Year    int
	Date    string
	Sport   string
	MedalsInfo
}

type Data []AthleteData

var data Data

func ParseInputData(path string) {
	jsonData, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	err = json.Unmarshal(jsonData, &data)
	if err != nil {
		panic(err)
	}
}

func ParseArgs() (string, string) {
	port := flag.String("port", "8080", "port")
	dataPath := flag.String("data", "./olympics/testdata/olympicWinners.json", "path to json with data")
	flag.Parse()
	return *port, *dataPath
}

type AthleteInfoResult struct {
	Name         string             `json:"athlete"`
	Country      string             `json:"country"`
	Medals       MedalsInfo         `json:"medals"`
	MedalsByYear map[int]MedalsInfo `json:"medals_by_year"`
}

func GetAthleteInfoByNameAndSport(name string, sport *string) (*AthleteInfoResult, error) {
	info := make([]*AthleteData, 0)
	for _, a := range data {
		if a.Name == name && (sport == nil || a.Sport == *sport) {
			info = append(info, &a)
		}
	}

	if len(info) == 0 {
		return nil, errors.New("Not found")
	}

	result := AthleteInfoResult{
		Name:         name,
		Country:      info[0].Country,
		MedalsByYear: make(map[int]MedalsInfo),
	}
	for _, infoByYear := range info {
		result.Medals.Add(infoByYear.MedalsInfo)
		result.MedalsByYear[infoByYear.Year] = infoByYear.MedalsInfo
	}

	return &result, nil
}

func AthleteInfo(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	name := r.URL.Query().Get("name")
	resultData, err := GetAthleteInfoByNameAndSport(name, nil)
	if err != nil {
		http.Error(w, "Not found", http.StatusNotFound)
		return
	}

	result, _ := json.Marshal(*resultData)

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write(result)
}

func TopAthletesInSport(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	sport := r.URL.Query().Get("sport")
	athletes := make(map[string]MedalsInfo)
	for _, a := range data {
		if a.Sport == sport {
			m := athletes[a.Name]
			m.Add(a.MedalsInfo)
			athletes[a.Name] = m
		}
	}

	if len(athletes) == 0 {
		http.Error(w, "Not found", http.StatusNotFound)
		return
	}

	medalsByName := make([]struct {
		name   string
		medals MedalsInfo
	}, 0)
	for name, medals := range athletes {
		medalsByName = append(medalsByName, struct {
			name   string
			medals MedalsInfo
		}{name: name, medals: medals})
	}
	sort.Slice(medalsByName, func(i, j int) bool {
		return medalsByName[j].medals.Less(medalsByName[i].medals) ||
			medalsByName[i].medals == medalsByName[j].medals && medalsByName[i].name < medalsByName[j].name
	})

	k := 3
	if r.URL.Query().Has("limit") {
		var err error
		k, err = strconv.Atoi(r.URL.Query().Get("limit"))
		if err != nil {
			http.Error(w, "Bad request", http.StatusBadRequest)
		}
	}
	medalsByName = medalsByName[:k]

	resultData := make([]AthleteInfoResult, 0)
	for _, a := range medalsByName {
		info, _ := GetAthleteInfoByNameAndSport(a.name, &sport)
		resultData = append(resultData, *info)
	}

	result, _ := json.Marshal(resultData)

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write(result)
}

func TopCountriesInYear(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	year, _ := strconv.Atoi(r.URL.Query().Get("year"))
	countries := make(map[string]MedalsInfo)
	for _, a := range data {
		if a.Year == year {
			m := countries[a.Country]
			m.Add(a.MedalsInfo)
			countries[a.Country] = m
		}
	}

	if len(countries) == 0 {
		http.Error(w, "Not Found", http.StatusNotFound)
	}

	medalsByCountry := make([]struct {
		Country string `json:"country"`
		MedalsInfo
	}, 0)
	for name, medals := range countries {
		medalsByCountry = append(medalsByCountry, struct {
			Country string `json:"country"`
			MedalsInfo
		}{Country: name, MedalsInfo: medals})
	}
	sort.Slice(medalsByCountry, func(i, j int) bool {
		return medalsByCountry[j].MedalsInfo.Less(medalsByCountry[i].MedalsInfo) ||
			medalsByCountry[i].MedalsInfo == medalsByCountry[j].MedalsInfo && medalsByCountry[i].Country < medalsByCountry[j].Country
	})

	k := 3
	if r.URL.Query().Has("limit") {
		var err error
		k, err = strconv.Atoi(r.URL.Query().Get("limit"))
		if err != nil {
			http.Error(w, "Bad request", http.StatusBadRequest)
		}
	}
	medalsByCountry = medalsByCountry[:min(k, len(medalsByCountry))]

	result, _ := json.Marshal(medalsByCountry)

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write(result)

}

func main() {
	port, path := ParseArgs()
	ParseInputData(path)

	http.HandleFunc("/athlete-info", AthleteInfo)
	http.HandleFunc("/top-athletes-in-sport", TopAthletesInSport)
	http.HandleFunc("/top-countries-in-year", TopCountriesInYear)
	log.Fatal(http.ListenAndServe("localhost:"+port, nil))
}
