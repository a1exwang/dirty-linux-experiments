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
    out = &fns[i]->forward(*out);
  }
  assert(out->size() == 1);
  acc_ = dynamic_cast<Loss*>(fns[fns.size()-1])->accuracy();
  loss_ = out->scalar();
}

void Net::backward() {
  for (int64_t i = fns.size() - 1; i > 0; i--) {
    const auto &input = fns[i-1]->output();
    auto &input_diff = fns[i-1]->output_diff();
    fns[i]->df(input_diff, input);

    if (world_size > 0) {
      MPI_Allreduce(MPI::IN_PLACE, input_diff.mutableData(), (int)input_diff.size(), MPI::FLOAT, MPI::SUM, MPI_COMM_WORLD);
      input_diff /= world_size;
    }
  }
}

void Net::applyUpdate() {
  for (auto &fn : fns) {
    fn->applyUpdates(lr);
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

}
