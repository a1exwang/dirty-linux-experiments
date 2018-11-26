#include <tensor.h>
#include <net.h>
#include <functions/data.h>
#include <functions/inner_product.h>
#include <functions/loss.h>
#include <functions/pure.h>
#include <utils/benchmark.h>

#include <mpi.h>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cmath>


using namespace asgd;
using namespace std;

tuple<int, int> initMPI() {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name_ of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  // Print off a hello world message
  printf("Initialized from processor %s, rank %d out of %d processors\n",
         processor_name, world_rank, world_size);

  MPI_Barrier(MPI_COMM_WORLD);
  return make_tuple(world_rank, world_size);
}

void endMPI() {
  MPI_Finalize();
}


int main(int argc, char **argv) {
  int iters = atoi(argv[1]);
  int64_t bs = atoi(argv[2]);
  bool use_mpi = false;
  if (argc >= 4) {
    use_mpi = (bool)atoi(argv[3]);
  }

  int world_rank, world_size;
  if (use_mpi) {
    tie(world_rank, world_size) = initMPI();
  }

  Benchmark benchmark("Net");

  auto sigmoid = [](Dtype x) -> Dtype { return Dtype(1.0) / (Dtype(1.0)+Dtype(exp(-x))); };
  auto dsigmoid = [](Dtype x, Dtype y) -> Dtype { return y * (1 - y); };

  auto relu = [](Dtype x) -> Dtype { if (x > 0) return x; else return 0; };
  auto drelu = [](Dtype x, Dtype y) -> Dtype { if (x > 0) return 1; else return 0; };

  vector<Function*> layers = {
      new Data("data", {bs, 28*28}, bs, use_mpi),
      new InnerProduct("fc1", 28*28, 10, bs),
//       new PureFunction("relu1", 512, bs, relu, drelu),
//       new InnerProduct("fc2", 512, 10, bs),
//       new PureFunction("relu2", 1200, bs, relu, drelu),
      // new InnerProduct("fc3", 1200, 10, bs),
      new PureFunction("sigmoid3", 10, bs, sigmoid, dsigmoid),
      new L2Norm("loss_", 10, bs, use_mpi),
  };

  int64_t test_size = 10;
  Net net(layers, 0.5, test_size, world_rank, world_size);
  net.setup();

  Dtype test_loss, test_acc;
  for (int it = 0; it < iters; it++) {
    benchmark.tick("forward", TimePoint::Attribute::Start);
    net.forward();
    benchmark.tick("forward", TimePoint::Attribute::End);

    Dtype loss, acc;
    tie(loss, acc) = net.status();

    benchmark.tick("backward", TimePoint::Attribute::Start);
    net.backward();
    benchmark.tick("backward", TimePoint::Attribute::End);

    benchmark.tick("applyUpdate", TimePoint::Attribute::Start);
    net.applyUpdate();
    benchmark.tick("applyUpdate", TimePoint::Attribute::End);

    if (world_rank == 0) {
      cout << "Iter = " << it << " Loss = " << loss << " Acc = " << std::setw(2) << acc*100 << "%" << endl;
//      if (it % test_size == 0) {
//        tie(test_loss, test_acc) = net.test(bs);
//        cout << "Test, loss = " << test_loss << " acc = " << test_acc << endl;
//      }
    }
    benchmark.pprint(cout);
//    cout << "------------------------" << std::endl;
  }
  if (use_mpi) {
    endMPI();
  }
}
