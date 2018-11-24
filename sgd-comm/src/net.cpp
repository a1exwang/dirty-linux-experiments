#include <net.h>

#include <cassert>

namespace asgd {


void Net::forward() {
  const Tensor &out = (*fns[0])(Tensor::zero());
  const Tensor *last_out = &out;

//  cout << "---------------------" << endl;
//  cout << "forward: " << fns[0]->name() << endl;
//  last_out->pprint(std::cout);
  for (int i = 1; i < fns.size(); i++) {
    auto last_in = last_out;
    last_out = &((*fns[i])(*last_out));

//    cout << "---------------------" << endl;
//    cout << "forward: " << fns[i]->name() << endl;
//    cout << "input: ";
//    last_in->pprint(std::cout);
//    cout << "output: ";
//    last_out->pprint(std::cout);
  }
  assert(last_out->size() == 1);
  loss_ = last_out->scalar();
}

void Net::backward() {
  auto loss_tensor = Tensor::scalar(1);
  const Tensor *output_diff = &loss_tensor;

  for (int64_t i = fns.size() - 1; i > 0; i--) {
    // FIXME leaky memory
    const auto &input = fns[i-1]->output();
    Tensor *input_diff = new Tensor(input.shape());
    fns[i]->df(*input_diff, *output_diff, input);
//    cout << "--------------------------" << endl;
//    cout << "backward: " << fns[i]->name() << " Grad: ";
//    input_diff->pprint(std::cout);
//    if (i != fns.size() - 1) {
//      delete output_diff;
//    }
    output_diff = input_diff;
  }
}

void Net::applyUpdate() {
  for (auto &fn : fns) {
    fn->applyUpdates(lr);
  }
}
}
