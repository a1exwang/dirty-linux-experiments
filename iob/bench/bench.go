package bench

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"sync/atomic"
	"syscall"
	"time"
)

type PrettyFileSizeData struct {
	name  string
	width int
}

func PrettyFileSize(val float64, zz bool) string {
	pfd1 := []PrettyFileSizeData{
		{name: "", width: 0},
		{name: "Ki", width: 1},
		{name: "Mi", width: 2},
		{name: "Gi", width: 3},
		{name: "Ti", width: 4},
	}
	pfd2 := []PrettyFileSizeData{
		{name: "", width: 0},
		{name: "K", width: 1},
		{name: "M", width: 2},
		{name: "G", width: 3},
		{name: "T", width: 4},
	}
	if zz {
		for _, pfd := range pfd1 {
			size1 := uint64(1) << uint64(10*(pfd.width+1))
			if val < float64(size1) {
				return fmt.Sprintf(
					"%.2f%s",
					float64(val)/float64(uint64(1)<<uint64(10*pfd.width)),
					pfd.name,
				)
			}
		}
		return fmt.Sprintf(
			"%.2f%s",
			float64(val)/float64(uint64(1)<<uint64(10*pfd1[len(pfd1)-1].width)),
			pfd1[len(pfd1)-1].name,
		)
	} else {
		for _, pfd := range pfd2 {
			size1 := uint64(math.Pow10(3 * (pfd.width + 1)))
			size2 := uint64(math.Pow10(3 * pfd.width))
			if val < float64(size1) {
				return fmt.Sprintf(
					"%.2f%s",
					float64(val)/float64(size2),
					pfd.name,
				)
			}
		}
		return fmt.Sprintf(
			"%.2f%s",
			float64(val)/float64(math.Pow10(3*(pfd2[len(pfd2)-1].width+1))),
			pfd2[len(pfd2)-1].name,
		)
	}
}

func Bench(workerCount int64, file string, write bool, writeSize int64, bs int64, wbs int64, benchSeconds float64, direct bool) {
	var (
		count int64
		bytes int64
		f     *os.File
		err   error
	)

	var openFlags int
	fmt.Printf("direct=%v\n", direct)
	if direct {
		openFlags = syscall.O_DIRECT
	}

	if write {
		f, err = os.OpenFile(file, os.O_RDWR|os.O_CREATE|openFlags, 0644)
		if err != nil {
			panic(err)
		}

		err = f.Truncate(0)
		if err != nil {
			panic(err)
		}
		total := int64(0)
		fmt.Printf("Writing...\n")
		for total < writeSize {
			data := make([]byte, wbs)
			n := wbs
			for n > 0 {
				ret, err := f.Write(data[wbs-n : wbs])
				if err != nil {
					panic(err)
				}
				n -= int64(ret)
			}
			total += wbs
			fmt.Printf("%sB Written\n", PrettyFileSize(float64(total), true))
		}
	} else {
		f, err = os.OpenFile(file, os.O_RDONLY|openFlags, 0644)
		if err != nil {
			panic(err)
		}
	}

	s, err := f.Stat()
	if err != nil {
		panic(err)
	}
	fileSize := s.Size()
	fmt.Printf("fileSize: %d\n", fileSize)

	for i := 0; i < int(workerCount); i++ {
		go func() {
			buffer := make([]byte, bs)
			random := rand.New(rand.NewSource(time.Now().UnixNano()))
			for {
				offset := random.Int63() % fileSize
				offset = offset / bs * bs
				n, err := f.ReadAt(buffer, offset)
				if err != nil {
					fmt.Fprintf(os.Stderr, "err=%v\n", err)
					panic(err)
				}
				atomic.AddInt64(&count, 1)
				atomic.AddInt64(&bytes, int64(n))
			}
		}()
	}

	startBenchTime := time.Now()
	for time.Now().Sub(startBenchTime).Seconds() < benchSeconds {
		lastCount := count
		lastBytes := bytes
		lastTime := time.Now()

		time.Sleep(time.Second * 1)

		thisCount := count
		thisBytes := bytes

		sec := time.Now().Sub(lastTime).Seconds()
		fmt.Printf("%sIO/s, %.2f MiB/s\n", PrettyFileSize(float64(thisCount-lastCount)/sec, false), float64(thisBytes-lastBytes)/1024.0/1024.0/sec)
	}
}
