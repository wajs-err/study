//go:build !solution

package main

import (
	"bytes"
	_ "embed"
	"encoding/csv"
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"text/tabwriter"

	"github.com/spf13/pflag"
)

type Args struct {
	Repository   *string
	Revision     *string
	OrderBy      *string
	UseCommitter *bool
	Format       *string
	Extensions   *[]string
	Languages    *[]string
	Exclude      *[]string
	RestrictTo   *[]string

	ExtensionsByLang map[string][]string
}

//go:embed configs/language_extensions.json
var ExtensionsJSON []byte

func parseFileExtensions() map[string][]string {
	type LanguageInfo struct {
		Name       string
		Extensions []string
	}

	data := make([]LanguageInfo, 0)
	if err := json.Unmarshal(ExtensionsJSON, &data); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	res := make(map[string][]string)
	for _, lang := range data {
		res[strings.ToLower(lang.Name)] = lang.Extensions
	}
	return res
}

func parseArgs() Args {
	var args Args
	args.Repository = pflag.String("repository", ".", "")
	args.Revision = pflag.String("revision", "HEAD", "")
	args.OrderBy = pflag.String("order-by", "lines", "")
	args.UseCommitter = pflag.Bool("use-committer", false, "")
	args.Format = pflag.String("format", "tabular", "")
	args.Extensions = pflag.StringSlice("extensions", []string{}, "")
	args.Languages = pflag.StringSlice("languages", []string{}, "")
	args.Exclude = pflag.StringSlice("exclude", []string{}, "")
	args.RestrictTo = pflag.StringSlice("restrict-to", []string{}, "")
	pflag.Parse()

	args.ExtensionsByLang = parseFileExtensions()
	return args
}

type AuthorInfoExtended struct {
	Name    string
	Lines   int
	Commits map[string]struct{}
	Files   map[string]struct{}
}

type AuthorInfo struct {
	Name    string `json:"name"`
	Lines   int    `json:"lines"`
	Commits int    `json:"commits"`
	Files   int    `json:"files"`
}

func checkFileExtensionsMatch(name string, extensions []string) bool {
	for _, e := range extensions {
		if strings.HasSuffix(name, e) {
			return true
		}
	}
	return len(extensions) == 0
}

func checkLanguagesMatch(name string, languages []string, extensionsByLang map[string][]string) bool {
	unknownLangs := 0
	for _, lang := range languages {
		extensions, ok := extensionsByLang[strings.ToLower(lang)]
		if !ok {
			unknownLangs++
			continue
		}
		if checkFileExtensionsMatch(name, extensions) {
			return true
		}
	}
	return len(languages) == unknownLangs
}

func checkGlobsMatch(name string, patterns []string) bool {
	for _, pattern := range patterns {
		match, err := filepath.Match(pattern, name)
		if err != nil {
			fmt.Println(err)
			os.Exit(2)
		}
		if match {
			return true
		}
	}
	return len(patterns) == 0
}

func checkExcludedGlobsMatch(name string, patterns []string) bool {
	if len(patterns) == 0 {
		return false
	}
	return checkGlobsMatch(name, patterns)
}

func walkGitTreeAndCollectData(revision, node string, linesCount map[string]AuthorInfoExtended, args *Args) {
	cmd := exec.Command("git", "ls-tree", "--full-tree", "-r", revision)
	cmd.Dir = *args.Repository
	out, err := cmd.Output()
	if err != nil {
		fmt.Println(err)
		os.Exit(3)
	}

	lines := bytes.Split(out, []byte{'\n'})
	for _, l := range lines[:len(lines)-1] {
		name := string(bytes.Split(l, []byte{'\t'})[1])
		if !checkFileExtensionsMatch(name, *args.Extensions) ||
			!checkLanguagesMatch(name, *args.Languages, args.ExtensionsByLang) ||
			!checkGlobsMatch(name, *args.RestrictTo) || checkExcludedGlobsMatch(name, *args.Exclude) {
			continue
		}
		runGitBlame(filepath.Join(node, name), linesCount, revision, *args.Repository, *args.UseCommitter)
	}
}

func runGitBlame(path string, linesCount map[string]AuthorInfoExtended, revision, dir string, useCommitter bool) {
	cmd := exec.Command("git", "blame", path, revision, "--porcelain")
	cmd.Dir = dir
	out, err := cmd.Output()
	if err != nil {
		fmt.Println(err)
		os.Exit(4)
	}

	if len(out) == 0 {
		runGitLog(path, linesCount, revision, dir, useCommitter)
		return
	}

	lines := bytes.Split(out, []byte{'\n'})
	lines = lines[:len(lines)-1]

	nameByCommit := make(map[string]string)

	for i := 0; i < len(lines); {
		s := bytes.Split(lines[i], []byte{' '})
		commit := string(s[0])
		countStr := s[3]
		count, _ := strconv.Atoi(string(countStr)) // TODO : redundant allocation

		if bytes.HasPrefix(lines[i+1], []byte{'a', 'u', 't', 'h', 'o', 'r'}) {
			if useCommitter {
				nameByCommit[commit] = string(lines[i+5][10:])
			} else {
				nameByCommit[commit] = string(lines[i+1][7:])
			}
			i += 11
		}
		for !bytes.HasPrefix(lines[i], []byte{'\t'}) {
			i++
		}
		i += 2*count - 1

		name := nameByCommit[commit]
		info, ok := linesCount[name]
		if !ok {
			info = AuthorInfoExtended{
				Name:    nameByCommit[commit],
				Lines:   count,
				Commits: map[string]struct{}{commit: {}},
				Files:   map[string]struct{}{path: {}},
			}
		} else {
			info.Lines += count
			info.Commits[commit] = struct{}{}
			info.Files[path] = struct{}{}
		}
		linesCount[name] = info
	}
}

func runGitLog(path string, linesCount map[string]AuthorInfoExtended, revision, dir string, useCommitter bool) {
	cmd := exec.Command("git", "log", revision, "--format=full", "--", path)
	cmd.Dir = dir
	out, err := cmd.Output()
	if err != nil {
		fmt.Println(err)
		os.Exit(128)
	}

	lines := bytes.Split(out, []byte{'\n'})

	line := 1
	if useCommitter {
		line = 2
	}

	p := bytes.Split(lines[line][8:], []byte{' '})
	p = p[:len(p)-1]

	name := string(bytes.Join(p, []byte{' '})) // TODO : redundant allocation
	commit := string(bytes.Split(lines[0], []byte{' '})[1])

	info, ok := linesCount[name]
	if !ok {
		info = AuthorInfoExtended{
			Name:    name,
			Lines:   0,
			Commits: map[string]struct{}{commit: {}},
			Files:   map[string]struct{}{path: {}},
		}
	} else {
		info.Commits[commit] = struct{}{}
		info.Files[path] = struct{}{}
	}
	linesCount[name] = info
}

func transformAndSort(m map[string]AuthorInfoExtended, orderBy string) []AuthorInfo {
	res := make([]AuthorInfo, 0, len(m))
	for _, info := range m {
		res = append(res, AuthorInfo{
			Name:    info.Name,
			Lines:   info.Lines,
			Commits: len(info.Commits),
			Files:   len(info.Files),
		})
	}

	switch orderBy {
	case "lines":
		sort.Slice(res, func(i, j int) bool {
			return res[i].Lines > res[j].Lines ||
				res[i].Lines == res[j].Lines && res[i].Commits > res[j].Commits ||
				res[i].Lines == res[j].Lines && res[i].Commits == res[j].Commits && res[i].Files > res[j].Files ||
				res[i].Lines == res[j].Lines && res[i].Commits == res[j].Commits && res[i].Files == res[j].Files && res[i].Name < res[j].Name
		})

	case "commits":
		sort.Slice(res, func(i, j int) bool {
			return res[i].Commits > res[j].Commits ||
				res[i].Commits == res[j].Commits && res[i].Lines > res[j].Lines ||
				res[i].Commits == res[j].Commits && res[i].Lines == res[j].Lines && res[i].Files > res[j].Files ||
				res[i].Commits == res[j].Commits && res[i].Lines == res[j].Lines && res[i].Files == res[j].Files && res[i].Name < res[j].Name
		})

	case "files":
		sort.Slice(res, func(i, j int) bool {
			return res[i].Files > res[j].Files ||
				res[i].Files == res[j].Files && res[i].Lines > res[j].Lines ||
				res[i].Files == res[j].Files && res[i].Lines == res[j].Lines && res[i].Commits > res[j].Commits ||
				res[i].Files == res[j].Files && res[i].Lines == res[j].Lines && res[i].Commits == res[j].Commits && res[i].Name < res[j].Name
		})

	default:
		os.Exit(5)
	}
	return res
}

func printResult(stats []AuthorInfo, format string) {
	switch format {
	case "tabular":
		w := tabwriter.NewWriter(os.Stdout, 1, 4, 1, ' ', 0)
		fmt.Fprintln(w, "Name\tLines\tCommits\tFiles")
		for _, info := range stats {
			fmt.Fprintf(w, "%v\t%v\t%v\t%v\n", info.Name, info.Lines, info.Commits, info.Files)
		}
		w.Flush()
	case "csv":
		w := csv.NewWriter(os.Stdout)
                _ = w.Write([]string{"Name", "Lines", "Commits", "Files"})
		for _, info := range stats {
			_ = w.Write([]string{info.Name, strconv.Itoa(info.Lines), strconv.Itoa(info.Commits), strconv.Itoa(info.Files)})
		}
		w.Flush()
	case "json":
		json, _ := json.Marshal(stats)
		fmt.Println(string(json))
	case "json-lines":
		for _, info := range stats {
			json, _ := json.Marshal(info)
			fmt.Println(string(json))
		}
	default:
		fmt.Println("Unknown format")
		os.Exit(6)
	}
}

func main() {
	args := parseArgs()

	m := make(map[string]AuthorInfoExtended)
	walkGitTreeAndCollectData(*args.Revision, "", m, &args)

	res := transformAndSort(m, *args.OrderBy)

	printResult(res, *args.Format)
}
