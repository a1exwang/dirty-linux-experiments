package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/spf13/cobra"
)

var (
	routeCount int64
)


func init() {
	cmdDefault.PersistentFlags().Int64Var(&routeCount, "routeCount", 0, "")
}

var cmdDefault = &cobra.Command{
	Use: "rio",
	Run: runDefault,
}



func runDefault(cmd *cobra.Command, args []string) {

	for i := int64(0); i < routeCount; i++ {
		path := fmt.Sprintf("/abc/%d", i)
		http.HandleFunc(path, func(w http.ResponseWriter, r *http.Request) {
			w.Write(make([]byte, 0))
		})
	}
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Write(make([]byte, 0))
	})

	err := http.ListenAndServe(":9999", nil)
	log.Fatal(err)
}

func main() {
	cmdDefault.Execute()
}
