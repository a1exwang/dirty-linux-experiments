package stats

import (
	"errors"
	"fmt"
	"io"
	"sort"
	"sync"
	"time"
)

type Point struct {
	Timestamp time.Time
	Duration  int64
	Bytes     int64
}

func CreatePoint() *Point {
	return &Point{
		Timestamp: time.Now(),
		Duration:  0,
		Bytes:     0,
	}
}

type Stat struct {
	mu            sync.Mutex
	data          [][]Point
	currentBuffer int
	startTime     time.Time
	endTime       time.Time
}

func NewStat() *Stat {
	return &Stat{
		data:          make([][]Point, 2),
		currentBuffer: 0,
	}
}

func quantile(values []float64, cutPoints []float64) ([]float64, error) {
	values1 := make([]float64, len(values))
	copy(values1, values)

	sort.Float64s(values1)
	ret := make([]float64, len(cutPoints))
	i := 0
	for _, cp := range cutPoints {
		var x float64
		if cp > 1 || cp < 0 {
			return nil, errors.New("invalid argument, cut point must be in [0, 1]")
		} else if cp == 1 {
			x = values1[len(values1)-1]
		} else {
			x = values1[int64(cp*float64(len(values1)))]
		}
		ret[i] = x
		i++
	}

	return ret, nil
}

func total(data []float64) float64 {
	sum := 0.0
	for _, v := range data {
		sum += float64(v)
	}
	return sum
}

func (stat *Stat) Tick(p *Point, bytes int64) {
	p.Duration = time.Now().Sub(p.Timestamp).Nanoseconds()
	p.Bytes = bytes

	stat.mu.Lock()
	defer stat.mu.Unlock()
	stat.data[stat.currentBuffer] = append(stat.data[stat.currentBuffer], *p)
}

var CutPoints = []float64{
	0.5,
	0.667,
	0.75,
	0.90,
	0.95,
	0.98,
	0.99,
	0.999,
	1,
}

func (stat *Stat) Start() {
	stat.startTime = time.Now()
}
func (stat *Stat) End() {
	stat.endTime = time.Now()
}

func (stat *Stat) Reset() {
	stat.mu.Lock()
	defer stat.mu.Unlock()
	stat.data[stat.currentBuffer] = make([]Point, 0)
	stat.startTime = time.Now()
}

func (stat *Stat) Print(w io.Writer) {
	stat.mu.Lock()
	startTime := stat.startTime
	endTime := stat.endTime
	data := make([]Point, len(stat.data[stat.currentBuffer]))
	copy(data, stat.data[stat.currentBuffer])
	stat.mu.Unlock()

	if len(data) == 0 {
		fmt.Fprintf(w, "No Requests")
		return
	}

	var rps, mbps float64
	var totalSeconds = float64(endTime.Sub(startTime)) / (1000 * 1000 * 1000)

	durations := make([]float64, len(data))
	bytes := make([]float64, len(data))
	i := 0
	for _, it := range data {
		durations[i] = float64(it.Duration)
		bytes[i] = float64(it.Bytes)
		i++
	}
	if totalSeconds != 0 {
		rps = float64(len(data)) / totalSeconds
		mbps = float64(total(bytes)) / 1024.0 / 1024.0 / totalSeconds
	}

	fmt.Fprintf(w, "Total Seconds: %.2f\n", totalSeconds)
	fmt.Fprintf(w, "Total Request: %d\n", len(data))
	fmt.Fprintf(w, "Request/s: %.2f\n", rps)
	fmt.Fprintf(w, "Transfer Rate: %.2fMB/s\n", mbps)
	fmt.Fprintf(w, "AverageDuration: %.2fms\n", total(durations)/float64(len(durations))/1000.0/1000.0)
	fmt.Fprintf(w, "Duration Quantile:\n")

	q, err := quantile(durations, CutPoints)
	if err != nil {
		fmt.Printf("Failed to calculate quantile")
		return
	}
	for i := 0; i < len(CutPoints); i++ {
		fmt.Fprintf(w, "%+8.2f%% %+8.2fms\n", CutPoints[i]*100.0, q[i]/1000.0/1000.0)
	}
	fmt.Fprintf(w, "\n")
}
