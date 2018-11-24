#include <net.h>
#include <functions/data.h>
#include <functions/loss.h>

#include <cassert>
#include <iostream>

using namespace std;

namespace asgd {


void Net::forward() {
  auto zero = Tensor::zero();
  const Tensor *out = &fns[0]->forward(zero);
  for (int i = 1; i < fns.size(); i++) {
    out = &fns[i]->forward(*out);
//    cout << "forward " << fns[i]->name() << ", output = "; out->pprint(cout);
  }
  assert(out->size() == 1);
  loss_ = out->scalar();
}

void Net::backward() {
  auto loss_tensor = Tensor::scalar(1);

  for (int64_t i = fns.size() - 1; i > 0; i--) {
    const auto &input = fns[i-1]->output();
    auto &input_diff = fns[i-1]->output_diff();
    fns[i]->df(input_diff, input);
//    cout << "input_diff: " << fns[i]->name();
//    input_diff.pprint(cout);
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

  for (int i = 1; i < fns.size() - 1; i++) {
    auto output = new Tensor(fns[i]->output().shape());
    (*fns[i])(*output, *last_out);
//    delete last_out;
    last_out = output;
  }
  return loss->test(*last_out);
}

}
