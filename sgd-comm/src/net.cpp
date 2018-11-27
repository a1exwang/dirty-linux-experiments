#include <net.h>
#include <functions/data.h>
#include <functions/loss.h>

#include <cassert>
#include <iostream>
#include <chrono>
#include <mpi.h>

using namespace std;

namespace asgd {


void Net::forward() {
  auto zero = Tensor::null();
  const Tensor *out = &fns[0]->forward(zero);
  for (int i = 1; i < fns.size(); i++) {
    benchmark.tick(fns[i]->name() + " forward", TimePoint::Attribute::Start);
    out = &fns[i]->forward(*out);

    benchmark.tick(fns[i]->name() + " forward", TimePoint::Attribute::End);
    if (((world_size > 0) && rank == 0) ||
        world_size == 0) {
//      cout << "out: " << fns[i]->name() << " " << std::endl;
//      out->pprint2(cout);
    }
  }
  assert(out->size() == 1);

  if (world_size > 0) {
    acc_ = dynamic_cast<Loss*>(fns[fns.size()-1])->accuracy();
    loss_ = out->scalar();
    acc_ /= world_size;
    MPI_Allreduce(MPI::IN_PLACE, &loss_, 1, MPIDATATYPE, MPI::SUM, MPI::COMM_WORLD);
    MPI_Allreduce(MPI::IN_PLACE, &acc_, 1, MPIDATATYPE, MPI::SUM, MPI::COMM_WORLD);
  } else {
    acc_ = dynamic_cast<Loss*>(fns[fns.size()-1])->accuracy();
    loss_ = out->scalar();
  }

}

void Net::backward(bool print) {

  for (int64_t i = fns.size() - 1; i > 0; i--) {

    const auto &input = fns[i - 1]->output();
    auto &input_diff = fns[i - 1]->output_diff();
    benchmark.tick(fns[i]->name() + " backward", [&]() {
      fns[i]->df(input_diff, input);
    });

    if (world_size > 0) {
      benchmark.tick(fns[i]->name() + " allreduce", [&]() {
        MPI_Allreduce(MPI::IN_PLACE, input_diff.mutableData(), (int) input_diff.size(), MPIDATATYPE, MPI::SUM,
                      MPI_COMM_WORLD);
        input_diff /= world_size;
      });
    }
//    if (((world_size > 0) && rank == 0) ||
//        world_size == 0) {
//      cout << "input_diff: " << fns[i]->name() << " " << std::endl;
//      input_diff.pprint2(cout);
//    }
  }
}

void Net::applyUpdate() {
  for (int i = 1; i < fns.size(); i++) {
    benchmark.tick(fns[i]->name() + " applyUpdate", [&]() {
      fns[i]->applyUpdates(fns[i-1]->output(), lr);
    });
  }
}

std::tuple<Dtype, Dtype> Net::test(int64_t test_size) {
  auto data = dynamic_cast<Data*>(fns[0]);
  auto loss = dynamic_cast<Loss*>(fns[fns.size() - 1]);
  const Tensor *last_out = &data->test(test_size);

  vector<Tensor*> tmpTensors(fns.size());
  for (int i = 1; i < fns.size() - 1; i++) {
    auto output = new Tensor(fns[i]->output().shape());
    tmpTensors[i] = output;
  }

  for (int i = 1; i < fns.size() - 1; i++) {
//    auto output = new Tensor(fns[i]->output().shape());
    auto output = tmpTensors[i];
    (*fns[i])(*output, *last_out);
    last_out = output;
  }

  auto ret = loss->test(*last_out);
  for (int i = 1; i < fns.size() - 1; i++) {
    delete tmpTensors[i];
  }
  return ret;
}

void Net::print() {
  if (world_size > 0) {
    for (int i = 0; i < world_size; i++) {
      if (i == rank) {
        cout << "at rank " << rank << " ";
        benchmark.pprint(cout);
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  } else {
    benchmark.pprint(cout);
  }
}

}
