package main // github.com/a1exwang/dirty-linux-experiments/golang-http/client

import (
	"io"
	"io/ioutil"
	"net/http"
	"os"
	"runtime"
	"time"

	"github.com/a1exwang/dirty-linux-experiments/golang-http/stats"

	"github.com/spf13/cobra"
)

var (
	threadCount     int64
	connectionCount int64
	concurrency     int64
	address         string
	url             string
)

func init() {
	clientCmd.PersistentFlags().Int64Var(&threadCount, "threadCount", 1, "")
	clientCmd.PersistentFlags().Int64Var(&connectionCount, "connectionCount", 1, "")
	clientCmd.PersistentFlags().Int64Var(&concurrency, "concurrency", 1, "")
	clientCmd.PersistentFlags().StringVar(&address, "address", ":9999", "")
}

var httpClient *http.Client

var clientCmd = &cobra.Command{
	Use: "httpben [flags] url",
	Run: runClient,
}

func makeRequest(workerId int, requestId int) (int64, error) {
	r, err := http.Get(url)
	if err != nil {
		return 0, err
	}

	n, err := io.Copy(ioutil.Discard, r.Body)
	return n, err
}

func runClient(cmd *cobra.Command, args []string) {
	if len(args) != 1 {
		cmd.Help()
		os.Exit(1)
	}
	url = args[0]

	runtime.GOMAXPROCS(int(threadCount))
	tp := http.DefaultTransport.(*http.Transport)
	tp.MaxIdleConns = int(connectionCount)
	httpClient = &http.Client{
		Transport: tp,
	}

	s := stats.NewStat()

	for i := 0; i < int(concurrency); i++ {
		go func() {
			j := 0
			for {
				p := stats.CreatePoint()

				bytesCopied, err := makeRequest(i, j)
				j++
				if err != nil {
					panic(err)
				}

				s.Tick(p, bytesCopied)
			}
		}()
	}

	for {
		s.Reset()
		time.Sleep(time.Second * 1)
		s.End()
		s.Print(os.Stdout)
	}
}

func main() {
	clientCmd.Execute()
}
