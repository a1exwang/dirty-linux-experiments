#include <tensor.h>
#include <net.h>
#include <functions/data.h>
#include <functions/inner_product.h>
#include <functions/loss.h>
#include <functions/pure.h>

#include <vector>
#include <iostream>
#include <cmath>


using namespace asgd;
using namespace std;


int main(int argc, char **argv) {
  int iters = atoi(argv[1]);

  auto sigmoid = [](Dtype x) -> Dtype { return Dtype(1.0) / (Dtype(1.0)+Dtype(exp(-x))); };
  auto dsigmoid = [](Dtype x, Dtype y) -> Dtype { return y * (1 - y); };

  auto relu = [](Dtype x) -> Dtype { if (x > 0) return x; else return 0; };
  auto drelu = [](Dtype x, Dtype y) -> Dtype { if (x > 0) return 1; else return 0; };

  int64_t bs = 1;

  vector<Function*> layers = {
      new Data("data", {bs, 28*28}, bs),
      new InnerProduct("fc1", 28*28, 10, bs),
      // new PureFunction("relu1", 1200, bs, relu, drelu),
      // new InnerProduct("fc2", 1200, 1200, bs),
      // new PureFunction("relu2", 1200, bs, relu, drelu),
      // new InnerProduct("fc3", 1200, 10, bs),
      new PureFunction("sigmoid3", 10, bs, sigmoid, dsigmoid),
      new L2Norm("loss_", 10, bs),
  };

//  Tensor *fake = new Tensor({bs, 4});
//  for (int i = 0; i < fake->size(); i++) {
//    (*fake)[i] = i - fake->size()/2;
//  }
//  vector<Function*> layers1 = {
////      new FakeData("data", fake),
//      new InnerProduct("fc1", 4, 10, bs),
//      new PureFunction("relu3", 10, bs, relu, drelu),
//      new L2Norm("loss_", 10, bs),
//  };

  Net net(layers, 0.1);
  net.setup();

  for (int it = 0; it < iters; it++) {
    net.forward();
    auto loss = net.loss();
    cout << "Iter = " << it << " Loss = " << loss << endl;
    net.backward();
    net.applyUpdate();
    cout << "------------------------" << std::endl;
  }
}
