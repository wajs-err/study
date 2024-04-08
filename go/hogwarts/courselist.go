//go:build !solution

package hogwarts

type NodeInfo struct {
	CourseName string
	TimeOut    int
}

func Dfs(current string, prereqs map[string][]string, in, out map[string]int, timer *int, visited map[string]bool, result *[]NodeInfo) {
	visited[current] = true
	*timer++
	in[current] = *timer

	for _, next := range prereqs[current] {
		if _, ok := visited[next]; !ok {
			Dfs(next, prereqs, in, out, timer, visited, result)
		}
	}

	*timer++
	out[current] = *timer
	*result = append(*result, NodeInfo{current, out[current]})
}

func GetCourseList(prereqs map[string][]string) []string {
	in := make(map[string]int)
	out := make(map[string]int)
	visited := make(map[string]bool)
	timer := 0
	result := make([]NodeInfo, 0, len(prereqs))

	for course := range prereqs {
		if _, ok := visited[course]; !ok {
			Dfs(course, prereqs, in, out, &timer, visited, &result)
		}
	}

	r := make([]string, 0, len(result))
	for _, course := range result {
		r = append(r, course.CourseName)
		for _, next := range prereqs[course.CourseName] {
			if out[next] >= course.TimeOut {
				panic("There are some cycles in courses graph")
			}
		}
	}

	return r
}
