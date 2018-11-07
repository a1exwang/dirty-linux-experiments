package main

import (
	"github.com/a1exwang/dirty-linux-experiments/iob/bench"
	"github.com/spf13/cobra"
)

var (
	workerCount  int64
	file         string
	write        bool
	writeSize    int64
	bs           int64
	wbs          int64
	benchSeconds float64
	direct       bool
)

func main() {
	var iobCmd = &cobra.Command{
		Use: "httpben [flags] url",
		Run: func(cmd *cobra.Command, args []string) {
			bench.Bench(workerCount, file, write, writeSize, bs, wbs, benchSeconds, direct)
		},
	}

	iobCmd.PersistentFlags().Int64Var(&workerCount, "workerCount", 1, "")
	iobCmd.PersistentFlags().StringVar(&file, "file", "", "")
	iobCmd.PersistentFlags().BoolVar(&write, "write", true, "")
	iobCmd.PersistentFlags().Int64Var(&writeSize, "writeSize", 2*1024*1024*1024, "")
	iobCmd.PersistentFlags().Int64Var(&bs, "bs", 4*1024, "")
	iobCmd.PersistentFlags().Int64Var(&wbs, "wbs", 64*1024*1024, "")
	iobCmd.PersistentFlags().Float64Var(&benchSeconds, "benchSeconds", 4, "")
	iobCmd.PersistentFlags().BoolVar(&direct, "direct", true, "")

	iobCmd.Execute()
}
