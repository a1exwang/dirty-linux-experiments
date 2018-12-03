#include "cache.hpp"
#include <cassert>

using namespace std;


int main() {
 std::vector<std::pair<std::set<int>, std::tuple<int64_t, int64_t, int64_t, int64_t, int64_t>>> table = {
     // total_images > total_cached_images
     {{}, {90, 8, 1, 2, 100}},

     // one replication
     {{0}, {0, 2, 1, 2, 4}},
     {{1}, {1, 2, 1, 2, 4}},
     {{0}, {2, 2, 1, 2, 4}},
     {{1}, {3, 2, 1, 2, 4}},

     // 2 replications
     {{0, 1}, {0, 3, 1, 4, 6}},
     {{2, 0}, {1, 3, 1, 4, 6}},
     {{1, 2}, {2, 3, 1, 4, 6}},
     {{0, 1}, {3, 3, 1, 4, 6}},
     {{2, 0}, {4, 3, 1, 4, 6}},
     {{1, 2}, {5, 3, 1, 4, 6}},

     // 2 replications + 1 * 3 replications
     {{0, 1, 2}, {0, 3, 1, 3, 4}},
     {{0, 1}, {1, 3, 1, 3, 4}},
     {{2, 0}, {2, 3, 1, 3, 4}},
     {{1, 2}, {3, 3, 1, 3, 4}},

     // 2 replications + 2 * 3 replications
     {{0, 1, 2}, {0, 5, 1, 2, 4}},
     {{3, 4, 0}, {1, 5, 1, 2, 4}},
     {{1, 2}, {2, 5, 1, 2, 4}},
     {{3, 4}, {3, 5, 1, 2, 4}},

 };

 int i = 0;
 for (auto test_case : table) {
   int64_t image_id, process_per_node, nodes, cached_images_per_process, total_images;
   std::tie(image_id, process_per_node, nodes, cached_images_per_process, total_images) =  test_case.second;

   auto expected = test_case.first;
   auto actual = caffe::DefaultDistributionHash(0, image_id, process_per_node, nodes, cached_images_per_process, total_images);
   if (expected != actual) {
     fprintf(stderr, "asserted failed\n");
     fprintf(stderr, "expected: (");
     for (auto x : expected) {
       fprintf(stderr, "%d, ", x);
     }
     fprintf(stderr, "), actual: (");
     for (auto x : actual) {
       fprintf(stderr, "%d, ", x);
     }
     fprintf(stderr, ")\n");
   }

   i++;
   printf("%d passed\n", i);
 }

}